/*
 * Copyright 2013 Schuyler Kylstra
 *
 * Comp 590-001 -- Fall 2013
 *
 *
 * Codename: Dori
 */


#include "GContext.h"
#include "GBitmap.h"
#include "GPaint.h"
#include "GColor.h"
#include "GRect.h"
#include "math.h"
#include "helperFunctions.h"
#include "ConversionMatrix.h"
#include <stack>




class GContext4 : public GContext {
public:
    GContext4() {
        mapMtx = *new ConversionMatrix();
    }
    
    GContext4(const GBitmap& bMap, const bool allocMem){
        Btmp = bMap;
        myMem = allocMem;
        mapMtx = *new ConversionMatrix();
        clear();
    }
    ~GContext4() {
        if (myMem){
            free(Btmp.fPixels);
            free(&mapMtx);
        }
    }
    
    void getBitmap(GBitmap*) const;
    
    void clear(const GColor&);
    
    void clear();
    
    static GContext* Create(const GBitmap&);
    
    static GContext* Create(int width, int height);
    
    void drawBitmap(const GBitmap&, float x, float y, const GPaint&);
    
    void drawRect(const GRect&, const GPaint&);
    
    void scale(float sx, float sy);
    
    void translate(float tx, float ty);
    
protected:
    
    void onSave();
    
    void onRestore();
    
private:
    GBitmap Btmp;
    bool myMem;
    helperFunctions helper;
    ConversionMatrix mapMtx;
    std::stack<ConversionMatrix> myStack;
};
/////////////////////////////////////////////////////////////////////////////////////////

void GContext4::translate(float tx, float ty){
    mapMtx.a3 += mapMtx.a1*tx + mapMtx.a2*ty;
    mapMtx.b3 += mapMtx.b2*ty + mapMtx.b1*tx;
}

void GContext4::scale(float sx, float sy){
    mapMtx.a1 *= sx;
    mapMtx.a2 *= sy;
    mapMtx.b1 *= sx;
    mapMtx.b2 *= sy;
}

void GContext4::onSave(){
    myStack.push(mapMtx);
}

void GContext4::onRestore(){
    mapMtx = myStack.top();
    myStack.pop();
}


void GContext4::drawBitmap(const GBitmap& lclBmp, float x, float y, const GPaint& paint){
    
    float alpha = paint.getAlpha();
    alpha = (alpha>1)?1:((alpha<0)?0:alpha);//bound alpha by 0 and 1
    uint8_t intAlpha = alpha*255 + .5;
    if(intAlpha != 0){
        
        float gTop = y, gLeft = x, gBottom = y+lclBmp.fHeight, gRight = x+lclBmp.fWidth;
        mapMtx.vectorMult(&gLeft, &gTop);
        mapMtx.vectorMult(&gRight, &gBottom);
        ConversionMatrix invMtx;
        mapMtx.getInverse(invMtx);
        
        helper.findIntersection(&gLeft, &gRight, &gTop, &gBottom, Btmp.fHeight, Btmp.fWidth);
        if((gLeft != gRight) && (gTop != gBottom)){ //if intersection surface area is zero, do nothing

            float topPixValue = helper.roundToInt(gTop), bottomPixValue = helper.roundToInt(gBottom);
            float gLeftStartValue = helper.roundToInt(gLeft), gRightPixValue = helper.roundToInt(gRight);
            
            uint32_t DstColor;
            uint32_t lclColor;
            
            for (float j = topPixValue; j< bottomPixValue; j++) {
                float lclYVal = j + .5;
                size_t gRowsDown = j*Btmp.fRowBytes;
                for (float i = gLeftStartValue; i <gRightPixValue; i++) {
                    float lclXVal = i + .5;
                    
                    char* gAddress = (char*) Btmp.fPixels + gRowsDown + (int) i;
                    DstColor = *((GPixel*) gAddress);
                    
                    invMtx.vectorMult(&lclXVal, &lclYVal);
                    lclColor = *(GPixel*) (char*) lclBmp.fPixels + floorf(lclYVal)*lclBmp.fRowBytes + floorf(lclXVal);
                    
                    
                    if (lclColor != 0) { //if there is something to blend
                        if(intAlpha == 1){
                            *((GPixel*) gAddress) = helper.blendColor(DstColor, lclColor);
                        }
                        else if( DstColor == 0 ) { //if destination alpha == 0
                            
                            *((GPixel*) gAddress) = helper.packARGB(lclColor, intAlpha);
                        }
                        else{
                            *((GPixel*) gAddress) = helper.blendColorWithAlpha(DstColor, lclColor, intAlpha);
                        }
                    }
                }
            }
        }
    }
}



void GContext4::drawRect(const GRect& rect, const GPaint& bucket){
    if((int)(bucket.getAlpha()*255 + .5) > 0){ //only do something if the alpha is greater than zero
        const GColor& color = bucket.getColor();
        if(helper.isLegalColor(color)){
            if(!rect.isEmpty()){
                float rTop=rect.fTop,rBottom=rect.fBottom,rLeft=rect.fLeft,rRight=rect.fRight;
                mapMtx.vectorMult(&rLeft, &rTop);
                mapMtx.vectorMult(&rRight, &rBottom);
                if(rLeft>rRight){
                    rLeft += rRight;
                    rRight = rLeft - rRight;
                    rLeft = rLeft - rRight;
                }
                if (rTop > rBottom){
                    rTop += rBottom;
                    rBottom = rTop - rBottom;
                    rTop = rTop - rBottom;
                }
                int left = helper.roundToInt(rLeft);
                int right = helper.roundToInt(rRight);
                int top = helper.roundToInt(rTop);
                int bottom = helper.roundToInt(rBottom);
                helper.findIntersection(&left, &right, &top, &bottom, Btmp.fHeight, Btmp.fWidth);
                uint32_t storedResult = 0x01;
                uint32_t storedDestination = 0x01;
                uint32_t DstColor;
                uint32_t rectColor = helper.packARGB(color);
                char* row = ((char*) Btmp.fPixels) + top*Btmp.fRowBytes;
                for(int j = top; j<bottom; j++){
                    for(int i=left ; i<right; i++){
                        DstColor = *((GPixel*) row + i);
                        if(storedDestination == DstColor){ //if we have the value already, don't do the math
                            *((GPixel*) row + i) = storedResult;
                        }
                        else{
                            storedDestination = DstColor;
                            storedResult = helper.blendColor(DstColor, rectColor);
                            *((GPixel*) row + i) = storedResult; //blends the two colors
                        }
                    }
                    row += Btmp.fRowBytes;
                }
            }
        }
    }
}



//matches the values of the passed bitmap to that of the instantiated one
void GContext4::getBitmap(GBitmap* bmp) const {
    *bmp = Btmp;
}

//completely fill the memory allocated with the premultiplied color
void GContext4::clear(const GColor& color){
    if((int) (color.fA*255 +.5) == 0){
        clear();
    }
    else if(helper.isLegalColor(color)){
        uint32_t pixColor = helper.packARGB(color);
        char* lclPixel = ((char*) Btmp.fPixels);
        if(Btmp.fWidth*4!=Btmp.fRowBytes){ //if we are not using pixels that are 4 bytes in size or if there is some other cruft in there
            for(int i = 0; i < Btmp.fHeight; i++ ){
                for( int j = 0; j< Btmp.fWidth; j++){
                    *((GPixel*)lclPixel + j) = pixColor; //steps accros a pixel in the memory allocation and assigns that memory slot the value stored in Btmp.fPixels
                }
                lclPixel +=  Btmp.fRowBytes;//Steps a rows worth of pixels in memory. Put it here so we dont step over the first row
            }
        }
        else{
            int pixels = Btmp.fWidth*Btmp.fHeight;
            for(int i = 0; i < pixels; i++){ //all the pixels are in one continuous space in memory so we just step for each pixel instead of for the rows then across the rows
                *((GPixel*)lclPixel) = pixColor;
                lclPixel += 4; //increment based on pixel size.
            }
        }
    }
}


void GContext4::clear(){
    uint32_t zero = 0x00;
    char* lclPixel = ((char*) Btmp.fPixels);
    int numPixels = Btmp.fWidth*Btmp.fHeight;
    for(int i = 0; i < numPixels; i++ ){
        *((GPixel*)lclPixel) = zero;
        lclPixel +=  4;//Steps one pixel in memory
    }
}





//This function assigns the passed bitmap to the instantiated GContext bitmap
GContext* GContext::Create(const GBitmap& bmp){
    if(bmp.fHeight<=0||bmp.fWidth<=0||bmp.fPixels==0x0){
        return NULL;
    }
    return new GContext4(bmp,false);
}




GContext* GContext::Create(int wid, int hei){
    if(wid<0||hei<0){
        return NULL;
    }
    else{
        GBitmap bm;
        const GBitmap& bmap = bm;
        bm.fRowBytes = wid*sizeof(GPixel);
        bm.fPixels = (GPixel*) malloc(hei*(bm.fRowBytes));
        bm.fHeight = hei;
        bm.fWidth = wid;
        return new GContext4(bmap,true);
    }
}



