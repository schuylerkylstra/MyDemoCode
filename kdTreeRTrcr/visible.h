/* 
*
*	visible.h
*   Created by Schuyler Kylstra
* 	11/12/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef OBJECT_visible_h
#define OBJECT_visible_h

#include "structures.h"


class visible
{
public:
	virtual float intersectionTime(ray * visionRay) = 0;

	virtual float * getColor() = 0;

	virtual spaceVector getNormal(spaceVector location) = 0;

	virtual surface * getSurface() = 0;

	virtual float getAlpha() = 0;
};

#endif