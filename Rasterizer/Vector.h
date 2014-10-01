/* 
*
*	Vector.h
*   Created by Schuyler Kylstra
* 	10/6/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/

#ifndef VIS_LVECTOR_H
#define VIS_LVECTOR_H

#define VECT_W 0

#include "Point.h"
#include <math.h>

class LVector : public LPoint
{
public:
	LVector()
	{
	coords[0] = 0;
	coords[1] = 0;
	coords[2] = 0;
	}

	LVector(float ix, float iy, float iz)
	{
		coords[0] = ix;
		coords[1] = iy;
		coords[2] = iz;
	}

	~LVector(){}

	void normalize();

	float dotProduct(LVector *);

	LVector crossProduct(LVector *);

	void scalarMult(float);

	void operator * (float);

	LVector vectorSum(LVector);

	LVector unitVect();

	LVector operator - (LVector);

	LVector operator + (LVector);
};

////////////////////////////////////////////////

void LVector::normalize()
{
	float iNorm = 1.0/(sqrt(coords[0]*coords[0] + coords[1]*coords[1] + coords[2]*coords[2]));
	setPosition(coords[0]*iNorm, coords[1]*iNorm, coords[2]*iNorm);
}


LVector LVector::unitVect()
{
	float iNorm = 1.0/(sqrt(coords[0]*coords[0] + coords[1]*coords[1] + coords[2]*coords[2]));
	return LVector(coords[0]*iNorm, coords[1]*iNorm, coords[2]*iNorm);
}


void LVector::operator * (float c)
{
	coords[0]	*= 	c;
	coords[1]	*= 	c;
	coords[2]	*= 	c;
}


float LVector::dotProduct(LVector * v)
{
	return coords[0]*v->coords[0] + coords[1]*v->coords[1] + coords[3]*v->coords[3]; 
}


LVector LVector::crossProduct(LVector * v)
{
	return LVector(coords[1]*v->coords[2] - coords[2]*v->coords[1], coords[2]*v->coords[0] - coords[0]*v->coords[2], coords[0]*v->coords[1] - coords[1]*v->coords[0]);
}


void LVector::scalarMult(float f)
{
	coords[0] = f*coords[0];
	coords[1] = f*coords[1];
	coords[2] = f*coords[2];
}


LVector LVector::vectorSum(LVector v)
{
	return LVector(coords[0] + v.coords[0], coords[1] + v.coords[1], coords[2] + v.coords[2]);
}


LVector LVector::operator - (LVector v)
{
	return LVector(coords[0] - v.coords[0], coords[1] - v.coords[1], coords[2] - v.coords[2]);
}


LVector LVector::operator + (LVector v)
{
	return LVector(coords[0] + v.coords[0], coords[1] + v.coords[1], coords[2] + v.coords[2]);
}


#endif