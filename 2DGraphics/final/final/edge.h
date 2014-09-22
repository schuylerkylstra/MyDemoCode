//
//  edge.h
//  Graphics
//
//  Created by Jonathan Kylstra on 11/10/13.
//  Copyright (c) 2013 Jonathan Kylstra. All rights reserved.
//

#ifndef Graphics_edge_h
#define Graphics_edge_h

#include "GPoint.h"


class edge{
public:
    GPoint point1;
    GPoint point2;
    float slope;
    float inverseSlope;
    
    void set(GPoint p1, GPoint p2){
        if(p1.fY > p2.fY){
            point2 = p1;
            point1 = p2;
        }
        else{
            point1 = p1;
            point2 = p2;
        }
        slope = (point1.fX - point2.fX)/(point1.fY - point2.fY);
        inverseSlope = 1/slope;
    }
    
    float getX(float y){
        return slope*y - slope*point2.fY + point2.fX;
    }
    
    float getY(float x){
        return inverseSlope*x - inverseSlope*point2.fX + point2.fY;
    }
    
    void clipToTop(){
        float newX = getX(0);
        point1.fX = newX;
        point1.fY = 0;
    }
    
    void clipToBottom(const int bottom){
        float newX = getX(bottom);
        point2.fX = newX;
        point2.fY = bottom;
    }
    
    void setToSide(const GBitmap bmap){
        if (point1.fX <= 0) {
            point1.fX = 0;
            point2.fX = 0;
        }
        else{
            point1.fX = bmap.fWidth;
            point2.fX = bmap.fWidth;
        }
        slope = (point1.fX - point2.fX)/(point1.fY - point2.fY);
        inverseSlope = 1/slope;
    }
    
    edge breakLeft(){
        edge newEdge;
        GPoint point2B;
        GPoint point1B;
        if (point1.fX < 0){
            point1B.set(0, point1.fY);
            point2B.set(0, getY(0));
            point1 = point2B;
        }
        else{
            point1B.set(0, point2.fY);
            point2B.set(0, getY(0));
            point2 = point2B;
        }
        newEdge.set(point1B, point2B);
        return newEdge;
    }
    edge breakRight(int width){
        edge newEdge;
        GPoint point2B;
        GPoint point1B;
        if (point1.fX > width){
            point1B.set(width, point1.fY);
            point2B.set(width, getY(width));
            point1 = point2B;
        }
        else{
            point1B.set(width, point2.fY);
            point2B.set(width, getY(width));
            point2 = point2B;
        }
        newEdge.set(point1B, point2B);
        return newEdge;

    }
    
    void jiggle_points(){
        point1.fY = floor(point1.fY*64)/64;
        point2.fY = floor(point2.fY*64)/64;
        set(point1, point2);
    }
    
    bool inline crossesHalfInt(){
        return (floor(point1.fY+.5) != floor(point2.fY+.5));
    }
    
    bool inline hasLargeSlope(){
        return (abs(slope) >= 1000);
        //return (floor(point1.fY) == floor(point2.fY))||(ceil(point1.fY) == ceil(point2.fY));
    }
};

#endif
