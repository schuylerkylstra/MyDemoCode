//
//  ConversionMatrix.h
//  Graphics
//
//  Created by Jonathan Kylstra on 10/22/13.
//  Copyright (c) 2013 Jonathan Kylstra. All rights reserved.
//

#ifndef __Graphics__ConversionMatrix__
#define __Graphics__ConversionMatrix__

#include <iostream>
#include <cmath>

class ConversionMatrix{
public:
    ConversionMatrix(){
        a1 = 1;
        a2 = 0;
        a3 = 0;
        b1 = 0;
        b2 = 1;
        b3 = 0;
    }
    
    ConversionMatrix(float x1, float x2, float x3, float y1, float y2, float y3){
        a1 = x1;
        a2 = x2;
        a3 = x3;
        b1 = y1;
        b2 = y2;
        b3 = y3;
    }
    
    ~ConversionMatrix(){
        
    }
    
    bool getInverse(ConversionMatrix&) const;
    void setValues(float, float, float, float, float, float);
    void vectorMult(float* xValue, float* yValue);
    void matrixMult(ConversionMatrix);
    float a1, a2, a3, b1, b2, b3;
    void makeIdentity();
    bool isIdentity();
    bool hasScale();
    bool hasRotation();
};

bool ConversionMatrix::isIdentity(){
    return (a1==1 && a2==0 && a3==0 && b1==0 && b2==1 && b3==0);
}

void ConversionMatrix::makeIdentity(){
    this->setValues(1.f, 0.f, 0.f, 0.f, 1.f, 0.f);
}


bool ConversionMatrix::getInverse(ConversionMatrix& matrix) const{ ////A^-1 = 1/det(A)*A^T
    float determinant = a1*b2 - a2*b1;
    if (determinant == 0) {
        return false;
    }
    float invDet = 1/determinant;
    matrix.setValues(b2*invDet, -a2*invDet, (a2*b3 - a3*b2)*invDet, -b1*invDet, a1*invDet, (a3*b1 - a1*b3)*invDet);
    return true;
    //this output works for arbitrary matrix with bottom row =  <0,0,1>
}

void ConversionMatrix::setValues(float x1, float x2, float x3, float y1, float y2, float y3){
    a1 = x1;
    a2 = x2;
    a3 = x3;
    b1 = y1;
    b2 = y2;
    b3 = y3;
}


void ConversionMatrix::vectorMult(float *xValue, float *yValue){
    float storedX = *xValue;
    *xValue = a1*(storedX) + a2*(*yValue) + a3;
    *yValue = b1*(storedX) + b2*(*yValue) + b3;
}

void ConversionMatrix::matrixMult(ConversionMatrix pMt){
    this->setValues(a1*pMt.a1 + a2*pMt.b1, a1*pMt.a2 + a2*pMt.b2, a1*pMt.a3 + a2*pMt.b3 + a3 , b1*pMt.a1 + b2*pMt.b1, b1*pMt.a2 + b2*pMt.b2, b1*pMt.a3 + b2*pMt.b3 + b3);
}

bool ConversionMatrix::hasScale(){
    return (a1 != 1|| b2 != 1);
}

bool ConversionMatrix::hasRotation(){
    return (a2 != 0 || b1 != 0);
}

#endif /* defined(__Graphics__ConversionMatrix__) */
