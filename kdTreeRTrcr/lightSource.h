/* 
*
*	lightSource.h
*   Created by Schuyler Kylstra
* 	11/12/14
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
	triple location;

public:

	lightSource()
	{

	}


	lightSource(color_RGBA color, triple loc)
	{
		colorEmitted = color;
		location = loc;
	}



	float * getColor()
	{
		return colorEmitted.getRGBColor();
	}



	triple inline getLocation()
	{
		return location;
	}
};

#endif