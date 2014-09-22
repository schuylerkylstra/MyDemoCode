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
#include <stack>


struct conversionMatrix{
    float a1;
    float a2;
    float a3;
    float b1;
    float b2;
    float b3;
    void vectorMult(float* xVal, float* yVal){
        *xVal = a1*(*xVal) + a2*(*yVal) + a3;
        *yVal = b1*(*xVal) + b2*(*yVal) + b3;
    }
};

class GContext4 : public GContext {
public:
    GContext4() {
        mapping.a1 = 1;
        mapping.a2 = 0;
        mapping.a3 = 0;
        mapping.b1 = 0;
        mapping.b2 = 1;
        mapping.b3 = 0;
    }
    
    GContext4(const GBitmap& bMap, const bool allocMem){
        Btmp = bMap;
        myMem = allocMem;
        clear();
        mapping.a1 = 1;
        mapping.a2 = 0;
        mapping.a3 = 0;
        mapping.b1 = 0;
        mapping.b2 = 1;
        mapping.b3 = 0;
    }
    ~GContext4() {
        if (myMem){
            free(Btmp.fPixels);
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
    conversionMatrix mapping;
    std::stack<conversionMatrix> myStack;
};
/////////////////////////////////////////////////////////////////////////////////////////

void GContext4::translate(float tx, float ty){
    mapping.a3 += mapping.a1*tx + mapping.a2*ty;
    mapping.b3 += mapping.b2*ty + mapping.b1*tx;
}

void GContext4::scale(float sx, float sy){
    mapping.a1 = mapping.a1*sx;
    mapping.a2 = mapping.a2*sy;
    mapping.b1 = mapping.b1*sx;
    mapping.b2 = mapping.b2*sy;
}

void GContext4::onSave(){
    myStack.push(mapping);
}

void GContext4::onRestore(){
    mapping = myStack.top();
    myStack.pop();
}


void GContext4::drawBitmap(const GBitmap& lclBmp, float x, float y, const GPaint& paint){
    
    float alpha = paint.getAlpha();
    alpha = (alpha>1)?1:((alpha<0)?0:alpha);//bound alpha by 0 and 1
    uint8_t intAlpha = alpha*255 + .5;
    if(intAlpha != 0){
        // first find the intersection
        float gTop = y, gLeft = x, gBottom = y+lclBmp.fHeight, gRight = x+lclBmp.fWidth;
        float inverseScaleX = 1;
        float inverseScaleY = 1;
            mapping.vectorMult(&gLeft, &gTop);
            mapping.vectorMult(&gRight, &gBottom);
            if(mapping.a1 != 1){
                inverseScaleX = abs(1/mapping.a1); //hide divides as much as possible
            }
            if(mapping.b2 != 1){
                inverseScaleY = abs(1/mapping.b2);
            }
        float lclTop = (gTop<0)?-gTop*inverseScaleY:(gTop>Btmp.fHeight)?(gTop-Btmp.fHeight*inverseScaleY):0;
        float lclLeft = (gLeft<0)?-gLeft*inverseScaleX:(gLeft>Btmp.fWidth)?(gLeft-Btmp.fWidth*inverseScaleX):0;
        helper.findIntersection(&gLeft, &gRight, &gTop, &gBottom, Btmp.fHeight, Btmp.fWidth); //even if directions are switched we will still move from gLeft to gRight and gTop to gBottom
        if((gLeft != gRight) && (gTop != gBottom)){ //if not visible, do nothing
            int topPixValue = helper.roundToInt(gTop), bottomPixValue = helper.roundToInt(gBottom);
            int gLeftStartValue = helper.roundToInt(gLeft), gRightPixValue = helper.roundToInt(gRight);
            int gIntervalPixelWidth = gRightPixValue - gLeftStartValue;
            int gIntervalPixelHeight = bottomPixValue - topPixValue;
            int lclLeftStartValue = helper.roundToInt(lclLeft);
            
            uint32_t DstColor;
            uint32_t lclColor;
            
            
            char* gRow = ((char*) Btmp.fPixels + topPixValue*Btmp.fRowBytes);
            char* lclRow = ((char*) lclBmp.fPixels + helper.roundToInt(lclTop)*lclBmp.fRowBytes);
            char* lclStartRow = lclRow;
            
            
            float lclXIncrementer = 0;
            float lclYIncrementer = 0;
            
            if (mapping.b2 > 0) { //if scale up & positive for Y
                for (int j = 0; j<gIntervalPixelHeight; j++) {
                    if (mapping.a1 > 0) {
                        for (int i = 0; i < gIntervalPixelWidth; i++) {
                            DstColor = *((GPixel*) gRow + gLeftStartValue + i);
                            lclColor = *((GPixel*) lclRow + lclLeftStartValue + helper.roundToInt(lclXIncrementer));
                            lclXIncrementer += inverseScaleX;
                            if (lclColor != 0) { //if source alpha is not zero
                                if(intAlpha == 1){
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.blendColor(DstColor, lclColor);
                                }
                                else if( DstColor == 0 ) { //if destination alpha == 0
                                    
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.packARGB(lclColor, intAlpha);
                                }
                                else{
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.blendColorWithAlpha(DstColor, lclColor, intAlpha);
                                }
                            }
                        }
                        lclXIncrementer = 0;
                    }
                    else{
                        for (int i = 0; i > gIntervalPixelWidth; i--) {
                            DstColor = *((GPixel*) gRow + gLeftStartValue + i);
                            lclColor = *((GPixel*) lclRow + lclLeftStartValue + helper.roundToInt(lclXIncrementer));
                            lclXIncrementer += inverseScaleX;
                            if (lclColor != 0) { //if source alpha is not zero
                                if(intAlpha == 1){
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.blendColor(DstColor, lclColor);
                                }
                                else if( DstColor == 0 ) { //if destination alpha == 0
                                    
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.packARGB(lclColor, intAlpha);
                                }
                                else{
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.blendColorWithAlpha(DstColor, lclColor, intAlpha);
                                }
                            }
                        }
                        lclXIncrementer = 0;
                    }
                    lclYIncrementer += inverseScaleY;
                    gRow += Btmp.fRowBytes;
                    lclRow = lclStartRow + helper.roundToInt(lclYIncrementer)*lclBmp.fRowBytes;
                }
            }
            else{
                for (int j = 0; j>gIntervalPixelHeight; j--) {
                    if (mapping.a1 > 0) {
                        for (int i = 0; i < gIntervalPixelWidth; i++) {
                            DstColor = *((GPixel*) gRow + gLeftStartValue + i);
                            lclColor = *((GPixel*) lclRow + lclLeftStartValue + helper.roundToInt(lclXIncrementer));
                            lclXIncrementer += inverseScaleX;
                            if (lclColor != 0) {
                                if(intAlpha == 1){
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.blendColor(DstColor, lclColor);
                                }
                                else if( DstColor == 0 ) {
                                    
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.packARGB(lclColor, intAlpha);
                                }
                                else{
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.blendColorWithAlpha(DstColor, lclColor, intAlpha);
                                }
                            }
                        }
                        lclXIncrementer = 0;
                    }
                    else{
                        for (int i = 0; i > gIntervalPixelWidth; i--) {
                            DstColor = *((GPixel*) gRow + gLeftStartValue + i);
                            lclColor = *((GPixel*) lclRow + lclLeftStartValue + helper.roundToInt(lclXIncrementer));
                            lclXIncrementer += inverseScaleX;
                            if (lclColor != 0) { //if source alpha is not zero
                                if(intAlpha == 1){
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.blendColor(DstColor, lclColor);
                                }
                                else if( DstColor == 0 ) { //if destination alpha == 0
                                    
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.packARGB(lclColor, intAlpha);
                                }
                                else{
                                    *((GPixel*) gRow + gLeftStartValue + i) = helper.blendColorWithAlpha(DstColor, lclColor, intAlpha);
                                }
                            }
                        }
                        lclXIncrementer = 0;
                    }
                    lclYIncrementer += inverseScaleY;
                    gRow += Btmp.fRowBytes; //steps a row of bytes in Btmp
                    lclRow = lclStartRow + helper.roundToInt(lclYIncrementer)*lclBmp.fRowBytes; //steps a row of bytes in lclBmp
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
                mapping.vectorMult(&rLeft, &rTop);
                mapping.vectorMult(&rRight, &rBottom);
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



