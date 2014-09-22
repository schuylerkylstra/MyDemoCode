//
//  ConversionMatrix.cpp
//  Graphics
//
//  Created by Jonathan Kylstra on 10/22/13.
//  Copyright (c) 2013 Jonathan Kylstra. All rights reserved.
//

#include "ConversionMatrix.h"


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
    
    void getInverse(ConversionMatrix&) const;
    void setValues(float, float, float, float, float, float);
    void vectorMult(float* xValue, float* yValue);
    void matrixMult(ConversionMatrix);
    float a1, a2, a3, b1, b2, b3;
};
/*
void ConversionMatrix::getInverse(ConversionMatrix& matrix) const{
    matrix.setValues(1/a1, 0.f, -a3, 0.f, 1/b2, -b3);
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
    *xValue = a1*(*xValue) + a2*(*yValue) + a3;
    *yValue = b1*(*xValue) + b2*(*yValue) + b3;
}

void ConversionMatrix::matrixMult(ConversionMatrix pMt){
    this->setValues(a1*pMt.a1 + a2*pMt.b1, a1*pMt.a2 + a2*pMt.b2, a1*pMt.a3 + a2*pMt.b3 + a3 , b1*pMt.a1 + b2*pMt.b1, b1*pMt.a2 + b2*pMt.b2, b1*pMt.a3 + b2*pMt.b3 + b3);
}
*/










