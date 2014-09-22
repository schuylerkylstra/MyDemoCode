//
//  helperFunctions.h
//  Graphics
//
//  Created by Jonathan Kylstra on 10/18/13.
//  Copyright (c) 2013 Jonathan Kylstra. All rights reserved.
//

#ifndef __Graphics__helperFunctions__
#define __Graphics__helperFunctions__

#include <iostream>
#include "GColor.h"
#include "GPixel.h"

class helperFunctions{
public:
    template <class T> T myMin(const T input1, const T input2){
        return (input1<input2)?input1:input2;
    }
    
    template <class T> T myMax(const T input1, const T input2){
        return (input1 > input2)?input1:input2;
    }
    
    uint8_t inline intMultBias255(uint32_t num1, uint32_t num2){
        return ((num1*num2) + 255) >>8;
    }
    
    uint32_t inline packARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b){
        return (a<<GPIXEL_SHIFT_A)|(r<<GPIXEL_SHIFT_R)|(g<<GPIXEL_SHIFT_G)|(b<<GPIXEL_SHIFT_B);
    }
    
    uint32_t inline packARGB(const GColor& pixColor){
        uint8_t a = pixColor.fA*255 + .5;
        uint8_t r = pixColor.fA*(pixColor.fR*255 + .5);
        uint8_t g = pixColor.fA*(pixColor.fG*255 + .5);
        uint8_t b = pixColor.fA*(pixColor.fB*255 + .5);
        return (a<<GPIXEL_SHIFT_A)|(r<<GPIXEL_SHIFT_R)|(g<<GPIXEL_SHIFT_G)|(b<<GPIXEL_SHIFT_B);
    }
    
    uint32_t inline packARGB( const uint32_t src, const uint8_t alpha){
        uint8_t a = (src>>24);
        uint8_t r = (src>>16);
        uint8_t g = (src>>8);
        uint8_t b = src;
        a = intMultBias255(a, alpha);
        r = intMultBias255(r, alpha);
        g = intMultBias255(g, alpha);
        b = intMultBias255(b, alpha);
        return (a<<GPIXEL_SHIFT_A)|(r<<GPIXEL_SHIFT_R)|(g<<GPIXEL_SHIFT_G)|(b<<GPIXEL_SHIFT_B);
    }
    
    uint32_t blendColorWithAlpha( const uint32_t dst, const uint32_t src, const uint8_t a){
        //dst block of values
        uint8_t dstA = (dst>>GPIXEL_SHIFT_A);
        uint8_t dstPMR = (dst>>GPIXEL_SHIFT_R);
        uint8_t dstPMG = (dst>>GPIXEL_SHIFT_G);
        uint8_t dstPMB = dst>>GPIXEL_SHIFT_B;
        //src block of values
        uint8_t srcA = src>>GPIXEL_SHIFT_A;
        uint8_t srcPMR = src>>GPIXEL_SHIFT_R;
        uint8_t srcPMG = src>>GPIXEL_SHIFT_G;
        uint8_t srcPMB = src>>GPIXEL_SHIFT_B;
        
        //apply alpha to src
        srcA = intMultBias255(a,srcA);
        srcPMR = intMultBias255(a, srcPMR);
        srcPMG = intMultBias255(a, srcPMG);
        srcPMB = intMultBias255(a, srcPMB);
        
        uint8_t antiAlpha = 0xFF - srcA;
        dstA = srcA + intMultBias255(antiAlpha, dstA);
        dstPMR = srcPMR + intMultBias255(antiAlpha, dstPMR);
        dstPMG = srcPMG + intMultBias255(antiAlpha, dstPMG);
        dstPMB = srcPMB + intMultBias255(antiAlpha, dstPMB);
        return packARGB(dstA, dstPMR, dstPMG, dstPMB);
    }
    
    
    static inline float roundToInt(float input){
        return floorf(input + .5);
    }
    
    uint32_t blendColor(const uint32_t dst, const uint32_t src){
        //all values are premultiplied coming in
        uint8_t dstA = (dst>>GPIXEL_SHIFT_A);
        uint8_t dstR = (dst>>GPIXEL_SHIFT_R);
        uint8_t dstG = (dst>>GPIXEL_SHIFT_G);
        uint8_t dstB = dst>>GPIXEL_SHIFT_B;
        //*********
        uint8_t srcA = src>>GPIXEL_SHIFT_A;
        uint8_t srcR = src>>GPIXEL_SHIFT_R;
        uint8_t srcG = src>>GPIXEL_SHIFT_G;
        uint8_t srcB = src>>GPIXEL_SHIFT_B;
        //*********
        uint8_t antiAlpha = 0xFF - srcA;
        dstA = srcA + intMultBias255(antiAlpha, dstA);
        dstR = srcR + intMultBias255(antiAlpha, dstR);
        dstG = srcG + intMultBias255(antiAlpha, dstG);
        dstB = srcB + intMultBias255(antiAlpha, dstB);
        return packARGB(dstA, dstR, dstG, dstB);
    }
    
    bool inline isLegalColor(const GColor& color){
        return !((color.fA<0) || (color.fR<0) || (color.fG<0) || (color.fB<0)) || !(color.fA>1) || !((color.fA<color.fR) || (color.fA<color.fB) || (color.fA<color.fG));
    }

    template <class T> void findIntersection(T* left, T* right, T* top, T* bottom, float height,float width ){
        if (*top> *bottom){
            *top += *bottom;
            *bottom = *top - *bottom;
            *top = *top - *bottom;
        }
        
        if (*left > *right) {
            *left += *right;
            *right = *left - *right;
            *left = *left - *right;
        }
        *top = myMax<T>(*top, 0);
        *bottom = myMin<T>(*bottom,height);
        *left = myMax<T>(*left,0);
        *right = myMin<T>(*right,width);
    }
    

    bool inline contextContains(float pixelVal, int contextDimension){
        return (0 <= pixelVal) && (pixelVal <= contextDimension);
    }
};

#endif /* defined(__Graphics__helperFunctions__) */
