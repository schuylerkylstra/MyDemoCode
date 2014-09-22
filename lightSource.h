/* 
*
*	lightSource.h
*   Created by Schuyler Kylstra
* 	9/9/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef ENV_lightSource_H
#define ENV_lightSource_H

#include "structures.h"

class lightSource
{

private:

	color_RGBA colorEmitted;
	tuple location;

public:

	lightSource()
	{

	}



	lightSource(color_RGBA color, tuple loc)
	{
		colorEmitted = color;
		location = loc;
	}



	float * getColor()
	{
		return colorEmitted.getRGBColor();
	}



	tuple inline getLocation()
	{
		return location;
	}
};

#endif