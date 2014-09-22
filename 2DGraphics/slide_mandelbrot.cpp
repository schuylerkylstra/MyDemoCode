//
//  CreeperSlide.cpp
//  Graphics
//
//  Created by Jonathan Kylstra on 12/10/13.
//  Copyright (c) 2013 Jonathan Kylstra. All rights reserved.
//

#include "GContext.h"
#include "GSlide.h"
#include "GBitmap.h"
#include "GColor.h"
#include "GPaint.h"
#include "GRandom.h"
#include "GTime.h"
#include <math.h>


class MandelbrotSlide: public GSlide {
    
public:
    GContext* context;
    
    MandelbrotSlide(){
        
    }
    
    static GSlide* Create(void*){
        return new MandelbrotSlide;
    }
    
protected:
    void onDraw(GContext* ctx){
        GPaint paint;

        ctx->clear(GColor::Make(1,0,0,0));
        
        ctx->translate(2.0, 2.0);
        ctx->scale(320, 240);
        for (float x = -2; x<2; x+=.00625) {
            for (float y = -2; y<2; y+=.00625) {
                testMadelbrot(x,y,ctx);
            }
        }
        
    }
    
    const char* onName(){
        return "Mandelbrot";
    }
    
private:
    
    void testMadelbrot(float x0, float y0, GContext* ctx){
        x0+=.003125;
        y0+=.003125;
        float length = (x0*x0 + y0*y0);
        float x = x0;
        float y = y0;
        
        if (length >= 4){
            return;
        }
        else{
            float red = 0.00048828125;
            GPaint paint;
            
            for(int i=1;i<2048;++i){
                x = x*x - y*y + x0;
                y = 2*x*y + y0;
                length = (x*x + y*y);
                if(sqrt(length) >= 2){
                    paint.setARGB(red, red, 0, 0);
                    ctx->drawRect(GRect::MakeLTRB(x0-.003125, y0-.003125, x0 + .003125, y0+.003125), paint);
                    return;
                }
                red = math.sin(i);
            }
            paint.setARGB(1,1,1,1);
            ctx->drawRect(GRect::MakeLTRB(x0-.003125, y0-.003125, x0 + .003125, y0+.003125),paint);
        }
    }
};

GSlide::Registrar CreeperSlide_reg(MandelbrotSlide::Create);