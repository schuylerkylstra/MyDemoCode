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
#include "GPixel.h"
#include "GColor.h"
#include "math.h"
#include "GIRect.h"

class GContext2 : public GContext {
private:
  GBitmap Btmp;
  bool myMem;
  uint32_t sRes;
  uint32_t sDst;
public:
  GContext2() {
    
  }
    
  GContext2(const GBitmap& bMap, const bool allocMem){
    Btmp = bMap;
    myMem = allocMem;
    sRes = 0x01;
    sDst = 0x01;
      clear(); //this clear sets every pixel to black so we aren't trying to blend with an illegal value
    
  }
  ~GContext2() {
    if (myMem)
      free(Btmp.fPixels);
  }
  bool isLegalColor(const GColor&);
    /**
     *  Return the information about the context's bitmap.
     */
  void getBitmap(GBitmap*) const;
    
    /**
     *  Set the entire context's pixels to the specified value.
     */
  void clear(const GColor&);
    
  void clear();
    
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
    
  void fillIRect(const GIRect&, const GColor&);
    
  uint32_t packARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
    
  uint32_t blendColor(const uint32_t, const GColor&);
    
};


bool inline GContext2::isLegalColor(const GColor& color){
  return !((color.fA<0) || (color.fR<0) || (color.fG<0) || (color.fB<0)) || !(color.fA>1) || !((color.fA<color.fR) || (color.fA<color.fB) || (color.fA<color.fG));
}


   
uint32_t GContext2::blendColor(const uint32_t dst, const GColor& src){
  //res.c = src.a*src.c + (1 - src.a)*dst.c*dst.a
  uint8_t alpha = (dst>>GPIXEL_SHIFT_A);
  uint8_t red = (dst>>GPIXEL_SHIFT_R);
  uint8_t green = (dst>>GPIXEL_SHIFT_G);
  uint8_t blue = dst;
  float preMR = src.fA*src.fR;
  float preMG = src.fA*src.fG;
  float preMB = src.fA*src.fB;
  alpha = (int) (src.fA*255 + .5) +((int) ((1 - src.fA)*alpha + .5));
  red = (int) (preMR*255+.5)+((int) ((1 - src.fA)*red + .5));
  green = (int) (preMG*255+.5) +((int) ((1 - src.fA)*green + .5));
  blue = (int) (preMB*255+.5) +((int) ((1 - src.fA)*blue + .5));
  sDst = dst;
  sRes = packARGB(alpha, red, green, blue);
  return sRes;
}



    
void GContext2::fillIRect(const GIRect& rect, const GColor& color){
  // first need to find where we can legally write to the bitmap
  if(color.fA > 0){ //only do something if the alpha is greater than zero
    if(isLegalColor(color)){
      int top=rect.fTop,bottom=rect.fBottom,left=rect.fLeft,right=rect.fRight;
      if(!rect.isEmpty()){
	if(rect.fTop <0){
	  top = 0;
	}
	if(rect.fLeft <0){
	  left = 0;
	}
	if(rect.fRight > Btmp.fWidth){
	  right = Btmp.fWidth;
	}
	if(rect.fBottom > Btmp.fHeight){
	  bottom = Btmp.fHeight;
	}
	uint32_t DstColor;
	char* row = ((char*) Btmp.fPixels) + top*Btmp.fRowBytes;
	//need to pass in the legal parameters not the given parameters
	for(int j = top; j<bottom; j++){
	  // char* row = ((char*) Btmp.fPixels) + j * Btmp.fRowBytes; //steps down a row in memory
	  for(int i=left ; i<right; i++){
	    DstColor = *((GPixel*) row + i);
	    if(sDst == DstColor){ //if we have the value already, don't do the math
	      *((GPixel*) row + i) = sRes;
	    }
	    else{
	      *((GPixel*) row + i) = blendColor(DstColor, color); //blends the two colors
	    }
	  }
	  row += Btmp.fRowBytes;
	}
	sDst = 0x01; //sets the stored destination to an impossible value after the rectangle is completed so it will not interfere with the next rectangle
      }
    }
  }
}




//matches the values of the passed bitmap to that of the instantiated one
void GContext2::getBitmap(GBitmap* bmp) const {
  *bmp = Btmp;
}





//completely fill the memory allocated with the premultiplied color
void GContext2::clear(const GColor& color){  
  //using uint8_t gives implicit truncation for numbers greater than 255
  if(color.fA == 0){
    clear();
  }
  else if(isLegalColor(color)){
    uint8_t alpha = color.fA*255 + .5;
    uint8_t PMred = color.fA*(color.fR*255 + .5);
    uint8_t PMgreen = color.fA*(color.fG*255 + .5);
    uint8_t PMblue = color.fA*(color.fB*255 + .5);
    uint32_t pixColor = packARGB( alpha, PMred, PMgreen, PMblue);
    char* row = ((char*) Btmp.fPixels);
    if(Btmp.fWidth*4!=Btmp.fRowBytes){ //if we are not using pixels that are 4 bytes in size or if there is some other cruft in there
      for(int i = 0; i < Btmp.fHeight; i++ ){
    	for( int j = 0; j< Btmp.fWidth; j++){
    	  *((GPixel*)row + j) = pixColor; //steps accros a pixel in the memory allocation and assigns that memory slot the value stored in Btmp.fPixels
    	}
    	row +=  Btmp.fRowBytes;//Steps a rows worth of pixels in memory. Put it here so we dont step over the first row 
      }
    }
    else{
      long pixels = Btmp.fWidth*Btmp.fHeight;
      for(int i = 0; i < pixels; i++){ //all the pixels are in one continuous space in memory so we just step for each pixel instead of for the rows then across the rows
	*((GPixel*)row) = pixColor;
	row += 4; //increment based on pixel size. here row is represents the pixel address not the pixel row address
      }
    }
  }
}


void GContext2::clear(){
  uint32_t zero = 0x00;
  char* row = ((char*) Btmp.fPixels);
  for(int i = 0; i < Btmp.fHeight; i++ ){
    for( int j = 0; j< Btmp.fWidth; j++){
      *((GPixel*)row + j) = zero; //steps accros a pixel in the memory allocation and assigns that memory slot the value stored in Btmp.fPixels                                             
    }
    row +=  Btmp.fRowBytes;//Steps a rows worth of pixels in memory                                                                                                                               
  }
}

//This function assigns the passed bitmap to the instantiated GContext bitmap
GContext* GContext::Create(const GBitmap& bmp){
  if(bmp.fHeight<=0||bmp.fWidth<=0||bmp.fPixels==0x0){
    return NULL;
  }
  return new GContext2(bmp,false);
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
    return new GContext2(bmap,true);
  }
}


uint32_t inline GContext2::packARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b){
  return 0x00|(a<<GPIXEL_SHIFT_A)|(r<<GPIXEL_SHIFT_R)|(g<<GPIXEL_SHIFT_G)|(b<<GPIXEL_SHIFT_B);
}






