#include <colors.h>

#include <gps.h>
#include <maps.h>
#include <geometry.h>
#include <TFT_eSPI_Ex.h>

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
    tft.printf("%f %f  SATELLITES: %d MODE: %d", pos.lng, pos.lat, pos.satellites, mode);
    // tft.print(pos.lng, 4);
    // tft.print(" "); tft.print(pos.lat, 4);
    // tft.print(" Sats: "); tft.print(pos.satellites);
    // tft.print(" M: "); tft.print(mode);
}

void tft_footer(int zoom)
{
    auto heigh = 20;
    tft.fillRect(0, SCREEN_HEIGHT - heigh, SCREEN_WIDTH, SCREEN_HEIGHT, CYAN);
    tft.setCursor(5, SCREEN_HEIGHT - 25, 2);
    tft.printf("ZOOM: %d/%d", zoom, MAX_ZOOM);
}

void tft_msg(const char* msg)
{
    tft.fillRect(0, 0, SCREEN_WIDTH, 25, CYAN);
    tft.setCursor(5,5,2);
    tft.println(msg);
}

void header_msg(const char* msg)
{
    tft.fillRect(0, 0, SCREEN_WIDTH, 25, YELLOWCLEAR);
    tft.setCursor(5,5,2);
    tft.print(msg);
}

inline int16_t toScreenCoord(const int32_t pxy, const int32_t screen_center_xy, int zoom_level) // work with primitives for performance
{
    return round((double)(pxy - screen_center_xy) / zoom_level) + (double)SCREEN_WIDTH / 2.0;
}

inline void toScreenPolygon(const Polygon source, const Point16& center, const int zoom, Polygon& result)
{
    result.color = source.color;

    result.bbox.min.x = toScreenCoord( source.bbox.min.x, center.x, zoom);
    result.bbox.min.y = toScreenCoord( source.bbox.min.y, center.y, zoom);
    result.bbox.max.x = toScreenCoord( source.bbox.max.x, center.x, zoom);
    result.bbox.max.y = toScreenCoord( source.bbox.max.y, center.y, zoom);

    result.points.clear();

    for (auto p : source.points) {
        result.points.push_back(Point16(
            toScreenCoord(p.x, center.x, zoom),
            toScreenCoord(p.y, center.y, zoom)));
    }
}

void drawMe(int mode)
{
    if (mode == DEVMODE_NAV) {
        // TODO: paint only in NAV mode
        tft.fillTriangle(
            SCREEN_WIDTH/2 - 4, SCREEN_HEIGHT/2 + 5, 
            SCREEN_WIDTH/2 + 4, SCREEN_HEIGHT/2 + 5, 
            SCREEN_WIDTH/2,     SCREEN_HEIGHT/2 - 6, 
            RED);
    } else {
        tft.fillCircle(
            SCREEN_WIDTH/2 - 4, SCREEN_HEIGHT/2 + 5, 3, RED);
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
        for (Polygon polygon : mblock->polygons)
        {
            if (zoom_level > polygon.maxzoom) continue;
            if (!polygon.bbox.intersects(screen_bbox_mc)) continue;

            new_polygon.color = polygon.color;
            new_polygon.bbox.min.x = toScreenCoord( polygon.bbox.min.x, screen_center_mc.x, zoom_level);
            new_polygon.bbox.min.y = toScreenCoord( polygon.bbox.min.y, screen_center_mc.y, zoom_level);
            new_polygon.bbox.max.x = toScreenCoord( polygon.bbox.max.x, screen_center_mc.x, zoom_level);
            new_polygon.bbox.max.y = toScreenCoord( polygon.bbox.max.y, screen_center_mc.y, zoom_level);
            
            new_polygon.points.clear();
            for (Point16 p : polygon.points){ // TODO: move to fill_polygon
                new_polygon.points.push_back( Point16(
                    toScreenCoord( p.x, screen_center_mc.x, zoom_level),
                    toScreenCoord( p.y, screen_center_mc.y, zoom_level)));
            }

            fillPolygon(tft, new_polygon);
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

                // log_d("drawWideLine (%d, %d) to (%d, %d) with color %2x\n",
                //     p1x, SCREEN_HEIGHT - p1y,
                //     p2x, SCREEN_HEIGHT - p2y,
                //     line.width/zoom_level ?: 1, line.color, line.color);
            
                tft.drawWideLine(
                    p1x, SCREEN_HEIGHT - p1y,
                    p2x, SCREEN_HEIGHT - p2y,
                    line.width/zoom_level ?: 1, line.color, line.color);
            }
        }
        log_d("Block lines done %i ms\n", millis()-block_time);
    }

    log_d("Total %i ms\n", millis()-total_time);

    drawMe(mode);

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