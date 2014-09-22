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
#include "edge.h"
#include "helperFunctions.h"
#include "ConversionMatrix.h"
#include <stack>
#include <queue>
#include <algorithm>



static bool OrderEdges(const edge e1,const edge e2){
    if(e1.point1.fY < e2.point1.fY){
        return true;
    }
    else if (e1.point1.fY == e2.point1.fY){
        return e1.point1.fX < e2.point1.fX;
    }
    return false;
}



class GContext5 : public GContext {
public:
    GContext5() {
        mapMtx.makeIdentity();
    }
    
    GContext5(const GBitmap& bMap, const bool allocMem){
        mapMtx.makeIdentity();
        Btmp = bMap;
        myMem = allocMem;
        clear();
    }
    ~GContext5() {
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
    
    void rotate(float radians);
    
    void drawTriangle(const GPoint vertices[3], const GPaint&) ;
    
    void drawConvexPolygon(const GPoint vertices[], int count, const GPaint&);
protected:
    
    void onSave();
    
    void onRestore();
    
private:
    //void fillPolygon(GAutoSArray<edge, 105> edges,uint32_t color);
    
    void fillContainedPolygon(edge*,uint32_t color, int size);
    
    int clampEdge(edge e,edge outerEdge, edge* edges, int j);
    
    //bool getEdges(edge*, edge*, edge*, const GPoint vertices[3]);
    
    void blitter_solidColor(int length, const uint32_t color, GPixel* start_address, uint32_t* stored_dst_color, uint32_t* stored_blend_color);
    
  //  void blitter_FromBitmap(float x, float y, char* startAddress, int length, const ConversionMatrix mtx, GBitmap& ctx, const GBitmap& lclBmp);
    
    GBitmap Btmp;
    bool myMem;
    helperFunctions helper;
    ConversionMatrix mapMtx;
    std::stack<ConversionMatrix> myStack;
};
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


void GContext5::translate(float tx, float ty){
    mapMtx.a3 += mapMtx.a1*tx + mapMtx.a2*ty;
    mapMtx.b3 += mapMtx.b2*ty + mapMtx.b1*tx;
}

void GContext5::scale(float sx, float sy){
    mapMtx.a1 *= sx;
    mapMtx.a2 *= sy;
    mapMtx.b1 *= sx;
    mapMtx.b2 *= sy;
}


void GContext5::rotate(float radians){
    mapMtx
}


void GContext5::onSave(){
    myStack.push(mapMtx);
}

 void GContext5::onRestore(){
    mapMtx = myStack.top();
    myStack.pop();
}


void GContext5::drawBitmap(const GBitmap& lclBmp, float x, float y, const GPaint& paint){
    
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
            if(mapMtx.getInverse(invMtx)){//set invMtx as the inversion matrix for the current transform if there is no inverse then the bitmap is not writeable
            
                if((left != right) && (top != bottom)){ //if intersection surface area is zero, do nothing
                    uint32_t DstColor;
                    uint32_t lclColor;
                    float lclYVal;
                    float lclXVal;
                    float transformedYVal;
                    char* gLocal = (char*) Btmp.fPixels + top*Btmp.fRowBytes;
                    char* lclLocal;// = (char*) lclBmp.fPixels;
                
                    if(intAlpha ==1){
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
                                    *((GPixel*) gLocal + i) = helper.blendColor(DstColor, lclColor);
                                }
                            }
                            gLocal += Btmp.fRowBytes;
                        }
                        restore();
                        return;
                    }
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
                                if( DstColor == 0 ) { //if destination alpha == 0
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

        }
        else{
            drawBitmapIdent(lclBmp,x,y,paint,intAlpha);
        }
    }
}



void GContext5::drawBitmapIdent(const GBitmap& lclBmp, float x, float y, const GPaint& paint, uint8_t intAlpha){
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
    
    if(intAlpha == 1){
        for(int j = top; j< bottom; j++){
            for (int i = 0; i < intervalWidth; i++) {
                DstColor = *((GPixel*) gRow + i);
                lclColor = *((GPixel*) lclRow + i);
                if (lclColor != 0) { //if there is something to blend
                        *((GPixel*) gRow + i) = helper.blendColor(DstColor, lclColor);
                }
            }
            gRow += Btmp.fRowBytes;
            lclRow += lclBmp.fRowBytes;
        }
        return;
    }
    for(int j = top; j< bottom; j++){
        for (int i = 0; i < intervalWidth; i++) {
            DstColor = *((GPixel*) gRow + i);
            lclColor = *((GPixel*) lclRow + i);
            if (lclColor != 0) { //if there is something to blend
                if( DstColor == 0 ) { //if destination alpha == 0
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


void GContext5::drawRect(const GRect& rect, const GPaint& bucket){
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
                uint32_t DstColor = 0x00;
                uint32_t rectColor = helper.packARGB(color);
                /*char* row = ((char*) Btmp.fPixels + top*Btmp.fRowBytes + left);
                int width = right - left;
                for(int j = top; j<bottom; j++){
                    //blitter_solidColor(width, Btmp, rectColor, row, &storedDestination, &storedResult, DstColor);
                    
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
                }*/
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
void GContext5::getBitmap(GBitmap* bmp) const {
    *bmp = Btmp;
}



//completely fill the memory allocated with the premultiplied color
void GContext5::clear(const GColor& color){
    if((int) (color.fA*255 +.5) == 0){
        clear();
        return;
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
            return;
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


void GContext5::clear(){
    uint32_t zero = 0x00;
    char* lclPixel = ((char*) Btmp.fPixels);
    int numPixels = Btmp.fWidth*Btmp.fHeight;
    if(Btmp.fWidth*4!=Btmp.fRowBytes){
        for(int i = 0; i < Btmp.fHeight; i++ ){
            for( int j = 0; j< Btmp.fWidth; j++){
                *((GPixel*)lclPixel + j) = zero;
            }
            lclPixel +=  Btmp.fRowBytes;
        }
    }
    else{
        for(int i = 0; i < numPixels; i++ ){
            *((GPixel*)lclPixel) = zero;
            lclPixel +=  4;//Steps one pixel in memory
        }
    }
}


GContext* GContext::Create(const GBitmap& bmp){
    if(bmp.fHeight<=0||bmp.fWidth<=0||bmp.fPixels==0x0){
        return NULL;
    }
    return new GContext5(bmp,false);
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
        return new GContext5(bmap,true);
    }
}


/////////////////////////////////////////////////
void GContext5::drawTriangle(const GPoint vertices[3], const GPaint& color){
    drawConvexPolygon(vertices, 3, color);
}
/////////////////////////////////////////////////

void GContext5::drawConvexPolygon(const GPoint vertices[], int count, const GPaint& paint){
    if (count < 3){
        return;
    }
    uint32_t pigment = helper.packARGB(paint.getColor());
    if (pigment == 0) {
        return;
    }
    GAutoSArray<GPoint, 64> pointArray(count);
    GAutoSArray<edge, 64> edges(count+4);
    edge iteratedEdge;
    edge outerEdge;
    GRect BMRect;
    BMRect.setLTRB(0, 0, Btmp.fWidth, Btmp.fHeight);
    
    for(int k = 0; k<count; k++){
        pointArray[k] = vertices[k];
        mapMtx.vectorMult(&pointArray[k].fX, &pointArray[k].fY);/// make the real points
    }
    
    GRect polyRect;
    polyRect.setBounds(pointArray, count);
    int loopLength = count -1;
    int j = 0;
    edge* edgePtr = edges.get();
    
    if (BMRect.contains(polyRect)){//dont have to clamp edges
        iteratedEdge.set(pointArray[0], pointArray[count-1]);
        
        
        for(int i = 0; i < loopLength; i++){
            //iteratedEdge.jiggle_points();
            if(iteratedEdge.crossesHalfInt()){
                if (iteratedEdge.withinOneRow()) {
                    iteratedEdge.jiggle_points();
                    if (iteratedEdge.crossesHalfInt()) {
                        edges[j++] = iteratedEdge;
                    }
                }
                else{
                    edges[j++] = iteratedEdge;
                }
            }
            iteratedEdge.set(pointArray[i], pointArray[i+1]);
        }
        if(iteratedEdge.crossesHalfInt()){
            if (iteratedEdge.withinOneRow()) {
                iteratedEdge.jiggle_points();
                if (iteratedEdge.crossesHalfInt()) {
                    edges[j++] = iteratedEdge;
                }
            }
            else{
                edges[j++] = iteratedEdge;
            }
        }
    }
    else{
        iteratedEdge.set(pointArray[0], pointArray[count-1]);
        for (int i = 0; i< loopLength; i++){
            j = clampEdge(iteratedEdge, outerEdge, edgePtr, j);
            iteratedEdge.set(pointArray[i], pointArray[i+1]);
        }
        j = clampEdge(iteratedEdge, outerEdge, edgePtr, j);

    }

    std::sort(edgePtr, edgePtr + j, OrderEdges);
    if (j>=2) {
        fillContainedPolygon(edgePtr, pigment, j);
    }
}






void GContext5::fillContainedPolygon(edge* edges, uint32_t color, int size){
    edge leftEdge = edges[0];
    edge rightEdge = edges[1];
    if (leftEdge.point2.fX > rightEdge.point2.fX) {
        GSwap(leftEdge, rightEdge);
    }

    uint32_t stored_blend_color = 0x01;
    uint32_t stored_dst_color = 0x01;

    
    float y0 = leftEdge.point1.fY;
    int y = floor(y0 + .5);
    float halfIntY = y + .5;
    
    
    float fLeftX = leftEdge.getX(halfIntY);
    float fRightX = rightEdge.getX(halfIntY);
    
    int newEdges = 0;
    int leftX = floor(fLeftX + .5);
    int rightX = floor(fRightX + .5);
    char* row =  ((char*)Btmp.fPixels + y*Btmp.fRowBytes);
    GPixel* startAddress;
    
    
    for (int edgeNumber = 1; edgeNumber < size;) {
        while (halfIntY <= leftEdge.point2.fY && halfIntY <= rightEdge.point2.fY) {
            startAddress = (GPixel*)row + leftX;
            blitter_solidColor(rightX - leftX, color, startAddress, &stored_dst_color, &stored_blend_color);
            
            halfIntY++;
            row+= Btmp.fRowBytes;
            fLeftX += leftEdge.slope;
            fRightX += rightEdge.slope;
            leftX = floor(fLeftX + .5);
            rightX = floor(fRightX + .5);
        }
        newEdges = 0;
        if (halfIntY > leftEdge.point2.fY && edgeNumber< size){
            leftEdge = edges[++edgeNumber];
            fLeftX = leftEdge.getX(halfIntY);
            leftX = floor(fLeftX + .5);
            newEdges++;
        }
        if (halfIntY > rightEdge.point2.fY && edgeNumber < size){
            rightEdge = edges[++edgeNumber];
            fRightX = rightEdge.getX(halfIntY);
            rightX = floor(fRightX + .5);
            newEdges++;
        }
        if(newEdges==2 && (leftEdge.point1.fX > rightEdge.point1.fX)){
            GSwap(leftEdge, rightEdge);
            GSwap(fLeftX, fRightX);
            //GSwap(fLeftX, fRightX);
            leftX = floor(fLeftX +.5);
            rightX = floor(fRightX + .5);
        }
    }
    
}

int GContext5::clampEdge(edge e,edge outerEdge, edge* edges, int j){
    if (helper.containedInBitmap(e,Btmp) && e.crossesHalfInt()) {
        if (e.withinOneRow()) {
            e.jiggle_points();
            if (e.crossesHalfInt()) {
               edges[j++] = e;
            }
        }
        else{
            edges[j++] = e;
        }
    }
    else if (!helper.outOfBoundsY(e,Btmp)&& e.crossesHalfInt()){
        if(helper.crossesTop(e)){
            e.clipToTop();
        }
        if(helper.crossesBottom(e,Btmp.fHeight)){
            e.clipToBottom(Btmp.fHeight);
        }
        if(helper.outOfBoundsX(e, Btmp)){
            e.setToSide(Btmp);
        }
        if(helper.crossesLeft(e)){
            outerEdge = e.breakLeft();
            if (outerEdge.crossesHalfInt()){
                edges[j++] = outerEdge;
            }
        }
        if(helper.crossesRight(e, Btmp.fWidth)){
            outerEdge = e.breakRight(Btmp.fWidth);
            if (outerEdge.crossesHalfInt()) {
                edges[j++] = outerEdge;
            }
        }
        if (e.crossesHalfInt()) {
            if (e.withinOneRow()) {
                e.jiggle_points();
                if (e.crossesHalfInt()) {
                    edges[j++] = e;
                }
            }
            else{
                edges[j++] = e;
            }
        }
    }
    return j;
}
    





////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////blitters/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////



void GContext5::blitter_solidColor(int length, const uint32_t color, GPixel* ctx_StartPixel, uint32_t* stored_dst_color, uint32_t* stored_blend_color){
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
}


/*
void GContext5::blitter_FromBitmap_alphaIsOne(float x, float y, char* startAddress, int length, ConversionMatrix inverse_matrix, GBitmap& ctx, const GBitmap& lclBmp, float convertedX, float convertedY){
    ////pass in the inverse matrix as mtx//////
    
    for (int i = 0; i < length; i++) {
        mtx.vectorMult(&x,&y);
        *((GPixel) startAddress + i) =
    }
}
 
 
void GContext5::blitter_FromBitmap(float x, float y, char* startAddress, int length, ConversionMatrix mtx, GBitmap& ctx, const GBitmap& lclBmp, float convertedX, float convertedY){
 
 
 
 }
 
 */

