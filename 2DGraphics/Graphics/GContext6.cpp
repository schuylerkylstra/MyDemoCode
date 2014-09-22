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



class GContext6 : public GContext {
public:
    GContext6() {
        mapMtx.makeIdentity();
    }
    
    GContext6(const GBitmap& bMap, const bool allocMem){
        mapMtx.makeIdentity();
        ctxBtmp = bMap;
        myMem = allocMem;
        clear();
    }
    ~GContext6() {
        if (myMem){
            free(ctxBtmp.fPixels);
        }
    }
    
    void getBitmap(GBitmap*) const;
    
    void clear(const GColor&);
    
    void clear();
    
    
    
    static GContext* Create(const GBitmap&);
    
    static GContext* Create(int width, int height);
    
    
    
    void drawRect(const GRect&, const GPaint&);
    
    void drawTriangle(const GPoint vertices[3], const GPaint&) ;
    
    void drawConvexPolygon(const GPoint vertices[], int count, const GPaint&);
    
    void drawBitmap(const GBitmap&, float x, float y, const GPaint&);
    
    void drawRotatedBitmap(const GBitmap&, float x, float y, const GPaint&, uint8_t alpha);

    
    
    
    
    void scale(float sx, float sy);
    
    void translate(float tx, float ty);
    
    void rotate(float radians);
    

protected:
    
    void onSave();
    
    void onRestore();
    
private:
    
    void drawBitmapIdent(const GBitmap&, float x, float y, const GPaint&, const uint8_t a);
    
    void drawRotatedRect(const GRect&, const GPaint&);
    
    void fillContainedPolygon(edge*,uint32_t color, int size);
    
    void fillContainedBitmap(edge*, GBitmap bm, int size, uint8_t alpha);
    
    int clampEdge(edge e,edge outerEdge, edge* edges, int j);
    
    void blitter_solidColor(int startX, int stopX, const uint32_t color, GPixel* start_address, uint32_t* stored_dst_color, uint32_t* stored_blend_color);

    void blitter_bitmap(int startX,int stopX, float halfIntY, GPixel* ctxStartPixel, GBitmap bitmap, uint8_t alpha, ConversionMatrix inverse);
    
    GBitmap ctxBtmp;
    bool myMem;
    helperFunctions helper;
    ConversionMatrix mapMtx;
    std::stack<ConversionMatrix> myStack;
};
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


void GContext6::translate(float tx, float ty){
    mapMtx.a3 += mapMtx.a1*tx + mapMtx.a2*ty;
    mapMtx.b3 += mapMtx.b2*ty + mapMtx.b1*tx;
}

void GContext6::scale(float sx, float sy){
    mapMtx.a1 *= sx;
    mapMtx.a2 *= sy;
    mapMtx.b1 *= sx;
    mapMtx.b2 *= sy;
}


void GContext6::rotate(float radians){
    float cs = cos(radians);
    float sn = sin(radians);
    ConversionMatrix rotateMtx;
    rotateMtx.setValues(cs, -sn, 0, sn, cs, 0);
    mapMtx.matrixMult(rotateMtx);
}


void GContext6::onSave(){
    myStack.push(mapMtx);
}

void GContext6::onRestore(){
    mapMtx = myStack.top();
    myStack.pop();
}


void GContext6::drawRect(const GRect& rect, const GPaint& bucket){
    if((int)(bucket.getAlpha()*255 + .5) > 0){ //only do something if the alpha is greater than zero
        const GColor& color = bucket.getColor();
        if(helper.isLegalColor(color)){
            if(!rect.isEmpty()){
                //check the matrix to see which subfunction to send it to
                if (mapMtx.hasRotation()) {
                    drawRotatedRect(rect, bucket);
                    return;
                }
                
                
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
                helper.findIntersection(&left, &right, &top, &bottom, ctxBtmp.fHeight, ctxBtmp.fWidth);
                uint32_t storedResult = 0x01;
                uint32_t storedDestination = 0x01;
                uint32_t DstColor = 0x00;
                uint32_t rectColor = helper.packARGB(color);
                char* row = ((char*) ctxBtmp.fPixels) + top*ctxBtmp.fRowBytes;
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
                    row += ctxBtmp.fRowBytes;
                }
            }
        }
    }
}



void GContext6::drawRotatedRect(const GRect& rect, const GPaint& color){

    GPoint leftTop,rightTop,leftBottom,rightBottom;
    leftTop.set(rect.fLeft, rect.fTop);
    rightTop.set(rect.fRight, rect.fTop);
    leftBottom.set(rect.fLeft, rect.fBottom);
    rightBottom.set(rect.fRight, rect.fBottom);
    const GPoint points[] = {leftTop, rightTop, rightBottom, leftBottom};
    drawConvexPolygon(points, 4, color);
}




void GContext6::drawTriangle(const GPoint vertices[3], const GPaint& color){
    drawConvexPolygon(vertices, 3, color);
}





void GContext6::drawConvexPolygon(const GPoint vertices[], int count, const GPaint& paint){
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
    GRect ctxRect;
    ctxRect.setLTRB(0, 0, ctxBtmp.fWidth, ctxBtmp.fHeight);
    
    for(int k = 0; k<count; k++){
        pointArray[k] = vertices[k];
        mapMtx.vectorMult(&pointArray[k].fX, &pointArray[k].fY);/// make the real points
    }
    
    GRect polyRect;
    polyRect.setBounds(pointArray, count);
    int j = 0;
    edge* edgePtr = edges.get();
    
    if (ctxRect.contains(polyRect)){//dont have to clamp edges
        for(int i = 0; i < count; i++){
            iteratedEdge.set(pointArray[i], pointArray[(i+1)%count]);
            if(iteratedEdge.crossesHalfInt()){
                if (iteratedEdge.hasLargeSlope()) {
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
    }
    else{
        for (int i = 0; i< count; i++){
            iteratedEdge.set(pointArray[i], pointArray[(i+1)% count]);
            j = clampEdge(iteratedEdge, outerEdge, edgePtr, j);
        }
    }
    
    std::sort(edgePtr, edgePtr + j, OrderEdges);
    if (j>=2) {
        fillContainedPolygon(edgePtr, pigment, j);
    }
}


void GContext6::drawBitmap(const GBitmap& lclBmp, float x, float y, const GPaint& paint){
    float alpha = paint.getAlpha();
    alpha = (alpha>1)?1:((alpha<0)?0:alpha);//bound alpha by 0 and 1
    uint8_t intAlpha = alpha*255 + .5;
    if(intAlpha != 0){
        if (!mapMtx.isIdentity()){
            if(mapMtx.hasRotation()){
                drawRotatedBitmap(lclBmp, x, y, paint, intAlpha);
                return;
            }
            ////////////////////finds the region in ctx that is written to//////////////////////////
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
            helper.findIntersection(&left, &right, &top, &bottom, ctxBtmp.fHeight, ctxBtmp.fWidth);
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
                    char* ctxPixMem = (char*) ctxBtmp.fPixels + top*ctxBtmp.fRowBytes;
                    char* bmpPixMem;
                    
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
                                bmpPixMem = (char*) lclBmp.fPixels + (int)transformedYVal*lclBmp.fRowBytes;
                                
                                DstColor = *((GPixel*) ctxPixMem + i);
                                lclColor = *((GPixel*) bmpPixMem + (int)lclXVal);
                                if (lclColor != 0) { //if there is something to blend
                                    *((GPixel*) ctxPixMem + i) = helper.blendColor(DstColor, lclColor);
                                }
                            }
                            ctxPixMem += ctxBtmp.fRowBytes;
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
                            bmpPixMem = (char*) lclBmp.fPixels + (int)transformedYVal*lclBmp.fRowBytes + (int) lclXVal*sizeof(GPixel);
                            DstColor = *((GPixel*) ctxPixMem + i);
                            lclColor = *((GPixel*) bmpPixMem);
                            if (lclColor != 0) { //if there is something to blend
                                if( DstColor == 0 ) { //if destination alpha == 0
                                    *((GPixel*) ctxPixMem + i) = helper.packARGB(lclColor, intAlpha);
                                }
                                else{
                                    *((GPixel*) ctxPixMem + i) = helper.blendColorWithAlpha(DstColor, lclColor, intAlpha);
                                }
                            }
                        }
                        ctxPixMem += ctxBtmp.fRowBytes;
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



void GContext6::drawBitmapIdent(const GBitmap& lclBmp, float x, float y, const GPaint& paint, uint8_t intAlpha){
    float gTop = y, gLeft = x, gBottom = y+lclBmp.fHeight, gRight = x+lclBmp.fWidth;
    int left = helper.roundToInt(gLeft);
    int right = helper.roundToInt(gRight);
    int top = helper.roundToInt(gTop);
    int bottom = helper.roundToInt(gBottom);
    int lclTop = (top<0)?-top:0;
    int lclLeft = (left<0)?-left:0;
    int intervalWidth = right - left;
    helper.findIntersection(&left, &right, &top, &bottom, ctxBtmp.fHeight, ctxBtmp.fWidth);
    uint32_t DstColor;
    uint32_t lclColor;
    char* gRow = (char*) ctxBtmp.fPixels + top*ctxBtmp.fRowBytes + left;
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
            gRow += ctxBtmp.fRowBytes;
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
        gRow += ctxBtmp.fRowBytes;
        lclRow += lclBmp.fRowBytes;
    }
}


void GContext6::drawRotatedBitmap(const GBitmap & bm, float x, float y, const GPaint& paint, uint8_t alpha){
    
    GPoint topLeft, topRight, bottomLeft, bottomRight;
    topLeft.set(0, 0);
    topRight.set(bm.fWidth, 0);
    bottomLeft.set(0, bm.fHeight);
    bottomRight.set(bm.fWidth, bm.fHeight);
    GPoint points[] = {topLeft, topRight, bottomRight, bottomLeft};
    
    edge edges[8];
    edge iteratedEdge;
    save();
    translate(x, y);
    for (int i = 0; i<4; ++i) {
        mapMtx.vectorMult(&points[i].fX, &points[i].fY);
    }
    
    
    GRect ctxRect, bmRect;
    ctxRect.setLTRB(0, 0, ctxBtmp.fWidth, ctxBtmp.fHeight);
    bmRect.setBounds(points, 4);
    
    int j = 0;
    
    if (ctxRect.contains(bmRect)) {
        for(int i = 0; i < 4; i++){
            iteratedEdge.set(points[i], points[(i+1)%4]);
            if(iteratedEdge.crossesHalfInt()){
                if (iteratedEdge.hasLargeSlope()) {
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
    }
    else{
        edge outerEdge;
        for (int i = 0; i < 4; ++i) {
            iteratedEdge.set(points[i], points[(i+1)%4]);
            j = clampEdge(iteratedEdge, outerEdge, edges, j);
        }
    }
    std::sort(edges, edges + j, OrderEdges);
    if (j>=2) {
        fillContainedBitmap(edges, bm, j, alpha);
    }
    restore();
}


//matches the values of the passed bitmap to that of the instantiated one
void GContext6::getBitmap(GBitmap* bmp) const {
    *bmp = ctxBtmp;
}



//completely fill the memory allocated with the premultiplied color
void GContext6::clear(const GColor& color){
    if((int) (color.fA*255 +.5) == 0){
        clear();
        return;
    }
    else if(helper.isLegalColor(color)){
        uint32_t pixColor = helper.packARGB(color);
        char* lclPixel = ((char*) ctxBtmp.fPixels);
        if(ctxBtmp.fWidth*4!=ctxBtmp.fRowBytes){ //if we are not using pixels that are 4 bytes in size or if there is some other cruft in there
            for(int i = 0; i < ctxBtmp.fHeight; i++ ){
                for( int j = 0; j< ctxBtmp.fWidth; j++){
                    *((GPixel*)lclPixel + j) = pixColor; //steps accros a pixel in the memory allocation and assigns that memory slot the value stored in Btmp.fPixels
                }
                lclPixel +=  ctxBtmp.fRowBytes;//Steps a rows worth of pixels in memory. Put it here so we dont step over the first row
            }
            return;
        }
        else{
            int pixels = ctxBtmp.fWidth*ctxBtmp.fHeight;
            for(int i = 0; i < pixels; i++){ //all the pixels are in one continuous space in memory so we just step for each pixel instead of for the rows then across the rows
                *((GPixel*)lclPixel) = pixColor;
                lclPixel += 4; //increment based on pixel size.
            }
        }
    }
}


void GContext6::clear(){
    uint32_t zero = 0x00;
    char* lclPixel = ((char*) ctxBtmp.fPixels);
    int numPixels = ctxBtmp.fWidth*ctxBtmp.fHeight;
    if(ctxBtmp.fWidth*4!=ctxBtmp.fRowBytes){
        for(int i = 0; i < ctxBtmp.fHeight; i++ ){
            for( int j = 0; j< ctxBtmp.fWidth; j++){
                *((GPixel*)lclPixel + j) = zero;
            }
            lclPixel +=  ctxBtmp.fRowBytes;
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
    return new GContext6(bmp,false);
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
        return new GContext6(bmap,true);
    }
}









void GContext6::fillContainedPolygon(edge* edges, uint32_t color, int size){
//I didn't have the time/patience to make a separate blitter class to pass into here. Hence the 2 identical methods.
    edge leftEdge = edges[0];
    edge rightEdge = edges[1];
    
    uint32_t stored_blend_color = 0x01;
    uint32_t stored_dst_color = 0x01;
    
    
    float y0 = leftEdge.point1.fY;
    int y = floor(y0 + .5);
    float halfIntY = y + .5;
    
    
    float fLeftX = leftEdge.getX(halfIntY);
    float fRightX = rightEdge.getX(halfIntY);
    
    if(fLeftX > fRightX){
        GSwap(leftEdge, rightEdge);
        GSwap(fLeftX, fRightX);
    }
    
    int newEdges = 0;
    int leftX = floor(fLeftX + .5);
    int rightX = floor(fRightX + .5);
    char* row =  ((char*)ctxBtmp.fPixels + y*ctxBtmp.fRowBytes);
    
    
    for (int edgeNumber = 1; edgeNumber < size;) {
        while (halfIntY <= leftEdge.point2.fY && halfIntY <= rightEdge.point2.fY) {
            blitter_solidColor(leftX, rightX, color, (GPixel*) row, &stored_dst_color, &stored_blend_color);
            halfIntY++;
            row+= ctxBtmp.fRowBytes;
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
            leftX = floor(fLeftX +.5);
            rightX = floor(fRightX + .5);
        }
    }
    
}


void GContext6::fillContainedBitmap(edge* edges, GBitmap bm, int size, uint8_t alpha){
    
    edge leftEdge = edges[0];
    edge rightEdge = edges[1];
    float y0 = leftEdge.point1.fY;
    int y = floor(y0 + .5);
    float halfIntY = y + .5;
    
    
    float fLeftX = leftEdge.getX(halfIntY);
    float fRightX = rightEdge.getX(halfIntY);
    if(fLeftX > fRightX){
        GSwap(leftEdge, rightEdge);
        GSwap(fLeftX, fRightX);
    }
    
    int newEdges = 0;
    int leftX = floor(fLeftX + .5);
    int rightX = floor(fRightX + .5);

    char* ctxRow =  ((char*)ctxBtmp.fPixels + y*ctxBtmp.fRowBytes);
    ConversionMatrix invMtx;
    mapMtx.getInverse(invMtx);
    
    for (int edgeNumber = 1; edgeNumber < size;) {
        while (halfIntY <= leftEdge.point2.fY && halfIntY <= rightEdge.point2.fY) {
            
            blitter_bitmap(leftX, rightX, halfIntY, (GPixel*) ctxRow, bm, alpha, invMtx);
            
            halfIntY++;
            ctxRow+= ctxBtmp.fRowBytes;
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
            leftX = floor(fLeftX +.5);
            rightX = floor(fRightX + .5);
        }
    }
    
}




int GContext6::clampEdge(edge e,edge outerEdge, edge* edges, int j){
    if (helper.containedInBitmap(e,ctxBtmp) && e.crossesHalfInt()) {
        if (e.hasLargeSlope()) {
            e.jiggle_points();
            if (e.crossesHalfInt()) {
                edges[j++] = e;
            }
        }
        else{
            edges[j++] = e;
        }
    }
    else if (!helper.outOfBoundsY(e,ctxBtmp)&& e.crossesHalfInt()){
        if(helper.crossesTop(e)){
            e.clipToTop();
        }
        if(helper.crossesBottom(e,ctxBtmp.fHeight)){
            e.clipToBottom(ctxBtmp.fHeight);
        }
        if(helper.outOfBoundsX(e, ctxBtmp)){
            e.setToSide(ctxBtmp);
        }
        if(helper.crossesLeft(e)){
            outerEdge = e.breakLeft();
            if (outerEdge.crossesHalfInt()){
                edges[j++] = outerEdge;
            }
        }
        if(helper.crossesRight(e, ctxBtmp.fWidth)){
            outerEdge = e.breakRight(ctxBtmp.fWidth);
            if (outerEdge.crossesHalfInt()) {
                edges[j++] = outerEdge;
            }
        }
        if (e.crossesHalfInt()) {
            if (e.hasLargeSlope()) {
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


void GContext6::blitter_solidColor(int startX, int stopX, const uint32_t color, GPixel* ctx_StartPixel, uint32_t* stored_dst_color, uint32_t* stored_blend_color){
    uint32_t dst_color;
    if (stopX - startX <= 0){
        return;
    }
    for(int i = startX; i < stopX; i++){
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


void GContext6::blitter_bitmap(int startX, int stopX, float halfIntY,  GPixel* ctxStartPixel, GBitmap bm, uint8_t alpha, ConversionMatrix inverseMtx){
    if ( stopX - startX == 0 ) {
        return;
    }
    float transformedY;
    float transformedX;
    uint32_t dstColor, srcColor;
    char* bmpPixMem;
    
    for(int i = startX; i<stopX; ++i){
        transformedY = halfIntY;
        transformedX = i + .5;
        inverseMtx.vectorMult(&transformedX, &transformedY);
        transformedX = floorf(transformedX);
        transformedY = floorf(transformedY);
        if (i == startX||i == stopX - 1) {
            transformedX = (transformedX<0)?0:(transformedX>=bm.fWidth)?bm.fWidth-1:transformedX;
        }
        transformedY = (transformedY<0)?0:(transformedY>=bm.fHeight)?bm.fHeight-1:transformedY;
        bmpPixMem = (char*) bm.fPixels + (int)transformedY*bm.fRowBytes;
        srcColor = *((GPixel*) bmpPixMem + ((int)transformedX));
        if (alpha != 255) {
            srcColor = helper.packARGB(srcColor, alpha);
        }
        dstColor = *(ctxStartPixel + i);
        if (dstColor==0) {
            *(ctxStartPixel + i) = srcColor;
        }
        else{
            *(ctxStartPixel + i) = helper.blendColor(dstColor, srcColor);
        }
    }
}



