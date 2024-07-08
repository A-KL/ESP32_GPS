#include <colors.h>

#include <gps.h>
#include <maps.h>
#include <geometry.h>

#include "../lib/conf.h"

#include <TFT_eSPI.h>

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <logs.h>
    #include <cassert>
#endif

#include "draw.h"

extern TFT_eSPI tft;
extern int mode;

// class MapViewport
// {
//     public:

//     private:
//         inline int16_t toScreenCoord(const int32_t pxy, const int32_t screen_center_xy, int zoom_level) // work with primitives for performance
//         {
//             return round((double)(pxy - screen_center_xy) / zoom_level) + (double)SCREEN_WIDTH / 2.0;
//         }
// }

void tft_header(Coord& pos)
{
    tft.fillRect(0, 0, SCREEN_WIDTH, 25, YELLOWCLEAR);
    tft.setCursor(5,5,2);
    tft.printf("%f %f  Sats: %d M: %d", pos.lng, pos.lat, pos.satellites, mode);
    // tft.print(pos.lng, 4);
    // tft.print(" "); tft.print(pos.lat, 4);
    // tft.print(" Sats: "); tft.print(pos.satellites);
    // tft.print(" M: "); tft.print(mode);
}

void tft_footer(const char* msg)
{
    auto heigh = 20;
    tft.fillRect(0, SCREEN_HEIGHT - heigh, SCREEN_WIDTH, heigh, CYAN);
    tft.setCursor(5, SCREEN_HEIGHT - 25, 2);
    tft.println(msg);
}

void tft_msg(const char* msg)
{
    tft.fillRect(0, 0, 240, 25, CYAN);
    tft.setCursor(5,5,2);
    tft.println(msg);
}

void header_msg(const char* msg)
{
    tft.fillRect(0, 0, 240, 25, YELLOWCLEAR);
    tft.setCursor(5,5,2);
    tft.print(msg);
}

inline int16_t toScreenCoord(const int32_t pxy, const int32_t screen_center_xy, int zoom_level) // work with primitives for performance
{
    return round((double)(pxy - screen_center_xy) / zoom_level) + (double)SCREEN_WIDTH / 2.0;
}

// scanline fill algorithm
void fill_polygon(const Polygon& p)
{
    int16_t maxy = p.bbox.max.y;
    int16_t miny = p.bbox.min.y;

    if (maxy >= SCREEN_HEIGHT) 
        maxy = SCREEN_HEIGHT-1;

    if (miny < 0) 
        miny = 0;

    if (miny >= maxy)
        return;

    int16_t nodeX[p.points.size()];

    //  Loop through the rows of the image.
    int16_t nodes, i , swap;
    for (auto pixelY=miny; pixelY <= maxy; pixelY++) {  //  Build a list of nodes.        
        nodes=0;
        for (int i=0; i < (p.points.size() - 1); i++) {
            if ((p.points[i].y < pixelY && p.points[i+1].y >= pixelY) ||
                (p.points[i].y >= pixelY && p.points[i+1].y < pixelY)) {
                    nodeX[nodes++] = 
                        p.points[i].x + double(pixelY-p.points[i].y)/double(p.points[i+1].y-p.points[i].y) * 
                        double(p.points[i+1].x-p.points[i].x);
                }
        }
        assert(nodes < p.points.size());

        //  Sort the nodes, via a simple “Bubble” sort.
        i=0;
        while (i < nodes-1) {   // TODO: rework
            if( nodeX[i] > nodeX[i+1]) {
                swap=nodeX[i]; nodeX[i]=nodeX[i+1]; nodeX[i+1]=swap; 
                i=0;  
            }
            else { i++; }
        }

        //  Fill the pixels between node pairs.
        for (i=0; i <= nodes-2; i+=2) {
            if( nodeX[i] > SCREEN_WIDTH) break;
            if( nodeX[i+1] < 0 ) continue;
            if (nodeX[i] < 0 ) nodeX[i] = 0;
            if (nodeX[i+1] > SCREEN_WIDTH) nodeX[i+1] = SCREEN_WIDTH;
            tft.drawLine(nodeX[i], SCREEN_HEIGHT - pixelY, nodeX[i+1], SCREEN_HEIGHT - pixelY, p.color);
        }
    }
}

void draw(ViewPort& viewPort, MemCache& memCache, int zoom_level, int mode)
{
    tft.fillScreen(BACKGROUND_COLOR);

    Polygon new_polygon;
    uint32_t total_time = millis();
    log_d("Draw start %i\n", total_time);

    int16_t p1x, p1y, p2x, p2y;

    for (MapBlock* mblock: memCache.blocks)
    {
        uint32_t block_time = millis();
        if( !mblock->inView) continue;

        // block to draw
        Point16 screen_center_mc = viewPort.center.toPoint16() - mblock->offset.toPoint16();  // screen center with features coordinates
        BBox screen_bbox_mc = viewPort.bbox - mblock->offset;  // screen boundaries with features coordinates
        
        ////// Polygons 
        for (Polygon polygon : mblock->polygons){
            if (zoom_level > polygon.maxzoom) continue;
            if (!polygon.bbox.intersects( screen_bbox_mc)) continue;
            new_polygon.color = polygon.color;
            new_polygon.bbox.min.x = toScreenCoord( polygon.bbox.min.x, screen_center_mc.x, zoom_level);
            new_polygon.bbox.min.y = toScreenCoord( polygon.bbox.min.y, screen_center_mc.y, zoom_level);
            new_polygon.bbox.max.x = toScreenCoord( polygon.bbox.max.x, screen_center_mc.x, zoom_level);
            new_polygon.bbox.max.y = toScreenCoord( polygon.bbox.max.y, screen_center_mc.y, zoom_level);
            
            new_polygon.points.clear();
            for( Point16 p : polygon.points){ // TODO: move to fill_polygon
                new_polygon.points.push_back( Point16(
                    toScreenCoord( p.x, screen_center_mc.x, zoom_level),
                    toScreenCoord( p.y, screen_center_mc.y, zoom_level)));
            }
            fill_polygon(new_polygon);
            
        }
        log_d("Block polygons done %i ms\n", millis()-block_time);
        block_time = millis();
        
        ////// Lines
        for( Polyline line : mblock->polylines){
            if( zoom_level > line.maxzoom) continue;
            if( !line.bbox.intersects( screen_bbox_mc)) continue;

            p1x = -1;
            for( int i=0; i < line.points.size() - 1; i++) {    //TODO optimize
                p1x = toScreenCoord( line.points[i].x, screen_center_mc.x, zoom_level); 
                p1y = toScreenCoord( line.points[i].y, screen_center_mc.y, zoom_level); 
                p2x = toScreenCoord( line.points[i+1].x, screen_center_mc.x, zoom_level); 
                p2y = toScreenCoord( line.points[i+1].y, screen_center_mc.y, zoom_level); 
                tft.drawWideLine(
                    p1x, SCREEN_HEIGHT - p1y,
                    p2x, SCREEN_HEIGHT - p2y,
                    line.width/zoom_level ?: 1, line.color, line.color);
            }
        }
        log_d("Block lines done %i ms\n", millis()-block_time);
    }
    log_d("Total %i ms\n", millis()-total_time);


    if (mode == DEVMODE_NAV) {
        // TODO: paint only in NAV mode
        tft.fillTriangle(
            SCREEN_WIDTH/2 - 4, SCREEN_HEIGHT/2 + 5, 
            SCREEN_WIDTH/2 + 4, SCREEN_HEIGHT/2 + 5, 
            SCREEN_WIDTH/2,     SCREEN_HEIGHT/2 - 6, 
            RED);
    } else {
        tft.fillCircle(
            SCREEN_WIDTH/2 - 4, SCREEN_HEIGHT/2 + 5, 5, RED);
    }

    log_d("Draw done! %i\n", millis());
}

// void stats(ViewPort& viewPort, MapBlock& mblock)
// {
//     Point32 screen_center_mc = viewPort.center - mblock.offset;  // screen center with features coordinates
//     BBox screen_bbox_mc = viewPort.bbox - mblock.offset;  // screen boundaries with features coordinates

//     ////// Polygons 
//     int in_screen = 0, in_map = 0,  points_total = 0;
//     for( Polygon polygon : mblock.polygons){
//         bool hit = false;
//         for( Point16 p : polygon.points){
//             points_total++;
//             if( screen_bbox_mc.containsPoint( p)) in_screen++;
//             // if( map_bbox_mc.contains_point( p)) in_map++;
//         }
//     }
//     log_i("Polygons points.  in_screen: %i, in_map: %i,  total: %i", in_screen, in_map, points_total);
    
//     ////// Lines 
//     in_screen = 0;
//     in_map = 0;
//     points_total = 0;
//     for( Polyline polyline : mblock.polylines){
//         for( Point16 p : polyline.points){
//             points_total++;
//             if( screen_bbox_mc.containsPoint( p)) in_screen++;
//             // if( map_bbox_mc.contains_point( p)) in_map++;
//         }
//     }
//     log_i("Lines points. in_screen: %i,  in_map: %i,  total: %i", in_screen, in_map, points_total);
// }