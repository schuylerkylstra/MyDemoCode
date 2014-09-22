//
//  blitter.h
//  Graphics
//
//  Created by Jonathan Kylstra on 12/2/13.
//  Copyright (c) 2013 Jonathan Kylstra. All rights reserved.
//

#ifndef Graphics_blitter_h
#define Graphics_blitter_h

#include <helperFunctions.h>
#include <

class blitter{
public:
    
    
    
};

class rectBlitter: public blitter{
    void blitter_solidColor(int length, const uint32_t color, GPixel* ctx_StartPixel, uint32_t* stored_dst_color, uint32_t* stored_blend_color){
        uint32_t dst_color;
        if (length == 0){
            return;
        }
        for(int i = 0; i < length; i++){
            dst_color = *((GPixel*)ctx_StartPixel + i);
            if( dst_color == *stored_dst_color){
                *((GPixel*)ctx_StartPixel + i) = *stored_blend_color;
            }
            else{
                *stored_blend_color = helper.blendColor(dst_color, color);
                *stored_dst_color = dst_color;
                *((GPixel*)ctx_StartPixel + i) = *stored_blend_color;
            }
        }
        *stored_blend_color = 0x01;
        *stored_dst_color = 0x01;
    }

};

class bitmapBlitter: public blitter {
    
    void blitter_bitmap(int length, const uint32_t color, GPixel* ctxStartPixel, GPixel* bmStartPixel, uint8_t alpha){
        
    }

};



#endif
