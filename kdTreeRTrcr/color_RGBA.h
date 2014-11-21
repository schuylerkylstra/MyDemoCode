/* 
*
*	color_RGBA.h
*   Created by Schuyler Kylstra
* 	11/12/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef META_colorRGBA_H
#define META_colorRGBA_H

#include "triple.h"


struct color_RGBA : triple
{

	float RGBA[4];

	color_RGBA()
	{
		RGBA[0] = 0.f;
		RGBA[1] = 0.f;
		RGBA[2] = 0.f;
		RGBA[3] = 0.f;
	}

	color_RGBA(triple p)
	{
		RGBA[0] = p.X;
		RGBA[1] = p.Y;
		RGBA[2] = p.Z;
		RGBA[3] = 0.f;
	}

	color_RGBA(float inR, float inG, float inB)
	{
		RGBA[0] = inR;
		RGBA[1] = inG;
		RGBA[2] = inB;
		RGBA[3] = 0.f;
	}


	color_RGBA(float inR, float inG, float inB, float inA)
	{
		RGBA[0] = inR;
		RGBA[1] = inG;
		RGBA[2] = inB;
		RGBA[3] = inA;
	}

	color_RGBA(float * ar)
	{
		for (int i = 0; i < 3; ++i)
		{
			RGBA[i] = ar[i];
		}
		RGBA[3] = 0.f;
	}


	color_RGBA scalarMult(float c)
	{
		if(c<0)
		{
			return *this;
		}
		return color_RGBA(RGBA[0]*c, RGBA[1]*c, RGBA[2]*c, RGBA[3]*c);
	}



	float * getRGBColor()
	{
		return RGBA;
	}

	void operator+(color_RGBA c)
	{
		for (int i = 0; i < 4; ++i)
		{
			RGBA[i]+= c.RGBA[i];
		}
	}

	void operator/(float div)
	{
		if(div<=0)
		{
			return;
		}
		for (int i = 0; i < 4; ++i)
		{
			RGBA[i] = RGBA[i]/div;
		}
	}



	color_RGBA operator*(float c)
	{
		if(c<0)
		{
			return *this;
		}
		return color_RGBA(RGBA[0]*c, RGBA[1]*c, RGBA[2]*c, RGBA[3]*c);
	}

	void gammaCorrect(float c)
	{
		// c = 1/c;
		for (int i = 0; i < 4; ++i)
		{
			RGBA[i] = pow(RGBA[i], c);
		}
	}


	void cap()
	{
		for (int i = 0; i < 4; ++i)
		{
			if(RGBA[i] > 1.0)
			{
				RGBA[i] = 1.0;
			}
		}
	}

	void print()
	{
		printf("CHECK2 %f, %f, %f\n",RGBA[0],RGBA[1],RGBA[2] );
	}
};



#endif