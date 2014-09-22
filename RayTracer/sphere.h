/* 
*
*	sphere.h
*   Created by Schuyler Kylstra
* 	9/9/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef OBJECT_sphere_H
#define OBJECT_sphere_H

#include "structures.h"
#include "visible.h"
#include <float.h>


class sphere : public visible
{

public:
	sphere(tuple p, float r, surface c)
	{
		center = p;
		radius = r;
		material = c;
	}


	sphere(tuple p, float r)
	{
		center = p;
		radius = r;
	}


	sphere()
	{

	}


	~sphere()
	{

	}


	tuple getCenter()
	{
		return center;
	}


	float * getColor()
	{
		return material.getRGBColor();
	}


	void setColor(surface * c)
	{
		material = *c;
	}


	void setColor(color_RGBA RGB, float aCo, float dCo, float sCo, int sPow)
	{
		material = surface(RGB, aCo, dCo, sCo, sPow);
	}


	surface * getSurface()
	{
		return & material;
	}


	float getRadius()
	{
		return radius;
	}


	float intersectionTime(ray * r) //override		/* right now this assumes the ray origin is (0, 0, 0) */
	{
		// center, origin (0,0,0), radius, ray r
		return quadEQ(2*((r->direction)*(r->origin) + -1*((r->direction)*center)) , (r->origin).getMagSqrd() + center.getMagSqrd() - 2*((r->origin)*center) - radius*radius);
	}


	spaceVector getNormal(spaceVector location)
	{
		spaceVector normal = location - center;	// I defined the - operator for a spacevector to a tuple
		normal.normalize();
		return normal;
	}



private:
	tuple center;
	surface material;
	float radius;


	float quadEQ(float b, float c) // a is always 1
	{
		float radical = b*b - 4*c;
		if (radical < 0.0)
		{
			return FLT_MAX;
		}
		radical = sqrt(radical);
		b = -b;

		float smallRoot = (b - radical)/2;
		float largeRoot = (b + radical)/2;

		if(smallRoot > largeRoot)
		{
			smallRoot += largeRoot;
			largeRoot = smallRoot - largeRoot;
			smallRoot -= largeRoot;
		}
		if(smallRoot >= 0)
		{
			return smallRoot;
		}
		else if (largeRoot >= 0)
		{
			return largeRoot;
		}
		else
		{
			return FLT_MAX;
		}
	}
};


#endif