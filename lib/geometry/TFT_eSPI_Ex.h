#pragma once

#include "geometry.h"

#include <TFT_eSPI.h>

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <logs.h>
    #include <cassert>
#endif

/* Scanline fill algorithm */
void fillPolygon(TFT_eSPI& tft, const Polygon& p)
{
    int16_t maxy = p.bbox.max.y;
    int16_t miny = p.bbox.min.y;

    auto SCREEN_HEIGHT = tft.height();
    auto SCREEN_WIDTH = tft.width();

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
