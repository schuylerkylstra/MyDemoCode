/* 
*
*	plane.h
*   Created by Schuyler Kylstra
* 	9/9/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef OBJECT_plane_H
#define OBJECT_plane_H

#include "structures.h"
#include "visible.h"
#include <float.h>


class plane : public visible
{

private:
	tuple point;
	spaceVector normalVector;
	surface material;

public:

	plane(tuple defPoint, spaceVector v)
	{
		point = defPoint;
		normalVector = v;
		normalVector.normalize();
	}


	plane(tuple defPoint, spaceVector v, surface s)
	{
		point = defPoint;
		normalVector = v;
		normalVector.normalize();
		material = s;
	}


	plane()
	{

	}


	~plane()
	{

	}


	void setColor(surface * c)
	{
		material = *c;
	}


	void setColor(color_RGBA RGB, float aCo, float dCo, float sCo, int sPow)
	{
		material = surface(RGB, aCo, dCo, sCo, sPow);
	}


	float * getColor() //// this should probably be in a different place
	{
		return material.getRGBColor();
	}


	surface * getSurface()
	{
		return & material;
	}


	float intersectionTime(ray * r) //override 
	{
		float divisor = normalVector.dotProduct(r->direction);
		if (divisor == 0.0)
		{
			return FLT_MAX;
		}
		float t = (normalVector.dotProduct(point) - normalVector.dotProduct(r->origin))/divisor;
		if(t >= 0)
		{
			return t;
		}
		return FLT_MAX;
	}


	spaceVector getNormal(spaceVector location)
	{
		return normalVector;
	}
};

#endif