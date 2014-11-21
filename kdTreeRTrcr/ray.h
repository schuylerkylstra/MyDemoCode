/* 
*
*	ray.h
*   Created by Schuyler Kylstra
* 	11/12/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef META_ray_H
#define META_ray_H

#include "triple.h"
#include "spaceVector.h"


struct ray 
{
	triple origin;
	spaceVector direction;
	float tMin;
	float tMax;


	ray()
	{
		tMin = 0.0;
		tMax = 100.f;
		origin = triple();
		direction = spaceVector();
	}


	ray(triple orig, spaceVector dir, float min, float max)
	{
		origin = orig;
		direction = dir;
		tMin = min;
		tMax = max;
	}

	ray(spaceVector orig, spaceVector dir, float min, float max)
	{
		ray(triple(orig.X, orig.Y, orig.Z), dir, min, max);
	}


	~ray()
	{

	}


	void inline setDirection(spaceVector newDir)
	{
		direction = newDir;
	}


	ray changeDirection(spaceVector delta)
	{
		return ray(origin, direction.vectorSum(delta), tMin, tMax);
	}


	void inline normalize()
	{
		direction.normalize();
	}

	ray inline normalized()
	{
		return ray(origin, direction.normalized(), 0.f, FLT_MAX);
	}

	ray inline operator * (float c)
	{
		return ray(origin, direction*c, tMin, tMax);
	}

};

#endif