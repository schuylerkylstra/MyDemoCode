/*
 * Copyright 2013 Schuyler Kylstra
 *
 * Comp 590-001 -- Fall 2013
 *
 */


#include "GContext.h"
#include "GBitmap.h"
#include "GPixel.h"
#include "GColor.h"
#include "math.h"

class GContext0 : public GContext {
private:
    GBitmap Btmp;
    bool myMem;
    
public:
    GContext0() {
        
    }
    
    GContext0(const GBitmap& bmp){
        Btmp.fPixels = bmp.fPixels;
        Btmp.fHeight = bmp.fHeight;
        Btmp.fRowBytes = bmp.fRowBytes;
        Btmp.fWidth = bmp.fWidth;
        myMem = false;
    }
    ~GContext0() {
        if (myMem)
            free(Btmp.fPixels);
    }
    
    /**
     *  Return the information about the context's bitmap.
     */
    void getBitmap(GBitmap*) const;
    
    /**
     *  Set the entire context's pixels to the specified value.
     */
    void clear(const GColor&);
    
    /**
     *  Create a new context that will draw into the specified bitmap. The
     *  caller is responsible for managing the lifetime of the pixel memory.
     *  If the new context cannot be created, return NULL.
     */
    static GContext* Create(const GBitmap&);
    
    /**
     *  Create a new context is sized to match the requested dimensions. The
     *  context is responsible for managing the lifetime of the pixel memory.
     *  If the new context cannot be created, return NULL.
     */
    static GContext* Create(int width, int height);
    
    
    uint32_t packARGB(int a, int r, int g, int b);
    
    void makeTrue();


};





void GContext0::getBitmap(GBitmap* bmp) const {
    bmp->fHeight = Btmp.fHeight;
    bmp->fWidth = Btmp.fWidth;
    bmp->fPixels = Btmp.fPixels;
    bmp->fRowBytes = Btmp.fRowBytes;
}




void GContext0::clear(const GColor& color){

  *Btmp.fPixels = packARGB((int) color.fA*255 + .5,(int) (color.fR*255 + .5)*color.fA, (int) (color.fG*255 + .5)*color.fA, (int) (color.fB*255 +.5)*color.fA);
    for(int i = 0; i < Btmp.fHeight; i++ ){
        char* row = ((char*) Btmp.fPixels) + i * Btmp.fRowBytes;
        for( int j = 0; j< Btmp.fWidth; j++){
            *((GPixel*)row + j) = *Btmp.fPixels;
        }
    }
}




GContext* GContext::Create(const GBitmap& bmp){
    if(bmp.fHeight<=0||bmp.fWidth<=0||bmp.fPixels==0x0){
        return NULL;
    }
    GContext0* map = new GContext0(bmp);
    return map;
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
        
        
        GContext0* context = new GContext0(bmap);
        context->makeTrue();
        return context;

    }

}


uint32_t inline GContext0::packARGB(int a, int r, int g, int b){
    if(a>255||r>255||g>255||b>255||a<0||r<0||g<0||b<0){
        return 0;
    }
    return 0|((uint32_t)a<<GPIXEL_SHIFT_A)|((uint32_t)r<<GPIXEL_SHIFT_R)|((uint32_t)g<<GPIXEL_SHIFT_G)|((uint32_t)b<<GPIXEL_SHIFT_B);
}

void GContext0::makeTrue(){
    myMem = true;
}






