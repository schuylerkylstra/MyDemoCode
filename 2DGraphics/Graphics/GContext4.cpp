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
        mapMtx.makeIdentity();
    }
    
    GContext4(const GBitmap& bMap, const bool allocMem){
        mapMtx.makeIdentity();
        Btmp = bMap;
        myMem = allocMem;
        clear();
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
    
    void drawBitmapIdent(const GBitmap&, float x, float y, const GPaint&, const uint8_t a);
    
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
        if (!mapMtx.isIdentity()){

            ////////////////////finds the region in GCon that is written to//////////////////////////
            float gTop = y, gLeft = x, gBottom = y +lclBmp.fHeight, gRight = x + lclBmp.fWidth;
            mapMtx.vectorMult(&gLeft, &gTop);
            mapMtx.vectorMult(&gRight, &gBottom);
            if(gLeft>gRight){
                gLeft += gRight;
                gRight = gLeft - gRight;
                gLeft = gLeft - gRight;
            }
            if (gTop > gBottom){
                gTop += gBottom;
                gBottom = gTop - gBottom;
                gTop = gTop - gBottom;
            }
            int left = helper.roundToInt(gLeft);
            int right = helper.roundToInt(gRight);
            int top = helper.roundToInt(gTop);
            int bottom = helper.roundToInt(gBottom);
            helper.findIntersection(&left, &right, &top, &bottom, Btmp.fHeight, Btmp.fWidth);
            ////////////////////////////////////////////////////////////////////////////////////////////////
            
            save();
            translate(x, y);//to include the x and y in the inverse
            ConversionMatrix invMtx;
            mapMtx.getInverse(invMtx);//set invMtx as the inversion matrix for the current transform
        
            if((left != right) && (top != bottom)){ //if intersection surface area is zero, do nothing
                uint32_t DstColor;
                uint32_t lclColor;
                float lclYVal;
                float lclXVal;
                float transformedYVal;
                char* gLocal = (char*) Btmp.fPixels + top*Btmp.fRowBytes;
                char* lclLocal;// = (char*) lclBmp.fPixels;
                
                
                
                ///////////need to check for clamping////////////////
                // if the pixel that is being written from is not in the actual bitmap then don't write it
                for(int j= top; j< bottom; j++){
                    lclYVal = j + .5f;
                    for(int i = left; i<right; i++){
                        transformedYVal = lclYVal;
                        lclXVal = i + .5f;
                        invMtx.vectorMult(&lclXVal, &transformedYVal);
                        if (j==top||j==bottom||i == left||i == right) {
                            helper.checkClamping(&lclXVal, &transformedYVal,lclBmp);
                        }
                        lclLocal = (char*) lclBmp.fPixels + (int)transformedYVal*lclBmp.fRowBytes + (int) lclXVal*sizeof(GPixel);

                        DstColor = *((GPixel*) gLocal + i);
                        lclColor = *((GPixel*) lclLocal);
                        if (lclColor != 0) { //if there is something to blend
                            if(intAlpha == 1){
                                *((GPixel*) gLocal + i) = helper.blendColor(DstColor, lclColor);
                            }
                            else if( DstColor == 0 ) { //if destination alpha == 0
                                
                                *((GPixel*) gLocal + i) = helper.packARGB(lclColor, intAlpha);
                            }
                            else{
                                *((GPixel*) gLocal + i) = helper.blendColorWithAlpha(DstColor, lclColor, intAlpha);
                            }
                        }

                    }
                    gLocal += Btmp.fRowBytes;
                }
            }
            restore();
        }
        else{
            drawBitmapIdent(lclBmp,x,y,paint,intAlpha);
        }
    }
}



void GContext4::drawBitmapIdent(const GBitmap& lclBmp, float x, float y, const GPaint& paint, uint8_t intAlpha){
    float gTop = y, gLeft = x, gBottom = y+lclBmp.fHeight, gRight = x+lclBmp.fWidth;
    int left = helper.roundToInt(gLeft);
    int right = helper.roundToInt(gRight);
    int top = helper.roundToInt(gTop);
    int bottom = helper.roundToInt(gBottom);
    int lclTop = (top<0)?-top:0;
    int lclLeft = (left<0)?-left:0;
    int intervalWidth = right - left;
    helper.findIntersection(&left, &right, &top, &bottom, Btmp.fHeight, Btmp.fWidth);
    uint32_t DstColor;
    uint32_t lclColor;
    char* gRow = (char*) Btmp.fPixels + top*Btmp.fRowBytes + left;
    char* lclRow = (char*) lclBmp.fPixels + lclTop*lclBmp.fRowBytes + lclLeft;
    
    
    for(int j = top; j< bottom; j++){
        for (int i = 0; i < intervalWidth; i++) {
            DstColor = *((GPixel*) gRow + i);
            lclColor = *((GPixel*) lclRow + i);
            if (lclColor != 0) { //if there is something to blend
                if(intAlpha == 1){
                    *((GPixel*) gRow + i) = helper.blendColor(DstColor, lclColor);
                }
                else if( DstColor == 0 ) { //if destination alpha == 0
                    
                    *((GPixel*) gRow + i) = helper.packARGB(lclColor, intAlpha);
                }
                else{
                    *((GPixel*) gRow + i) = helper.blendColorWithAlpha(DstColor, lclColor, intAlpha);
                }
            }
        }
        
        gRow += Btmp.fRowBytes;
        lclRow += lclBmp.fRowBytes;
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



