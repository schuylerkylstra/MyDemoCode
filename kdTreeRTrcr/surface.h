/* 
*
*	surface.h
*   Created by Schuyler Kylstra
* 	11/12/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef META_surface_H
#define META_surface_H

#include "triple.h"
#include "color_RGBA.h"

struct surface
{

	color_RGBA ambientColor;
	float ambientCoefficient;

	color_RGBA diffuseColor;
	float diffuseCoefficient;

	color_RGBA specularColor;
	float specularCoefficient;
	int specularPower;

	float alpha; 


	surface()
	{
		ambientCoefficient = .2;
		diffuseCoefficient = 1.0;
		specularCoefficient = 0;
		specularPower = 0;
		alpha = 0;
		ambientColor = color_RGBA(1.0, 1.0, 1.0, 0.f)*ambientCoefficient;
		diffuseColor = color_RGBA(1.0, 1.0, 1.0, 0.f)*diffuseCoefficient;
		specularColor = color_RGBA(1.0, 1.0, 1.0, 0.f)*specularCoefficient;
		
	}

	surface(float a)
	{
		ambientCoefficient = .2;
		diffuseCoefficient = 1.0;
		specularCoefficient = 0;
		specularPower = 0;
		alpha = a;
		ambientColor = color_RGBA(1.0, 1.0, 1.0, 0.f)*ambientCoefficient;
		diffuseColor = color_RGBA(1.0, 1.0, 1.0, 0.f)*diffuseCoefficient;
		specularColor = color_RGBA(1.0, 1.0, 1.0, 0.f)*specularCoefficient;
	}


	surface(color_RGBA color, float amCo, float diCo, float spCo, int spPo, float a)
	{
		ambientColor = color*amCo;
		diffuseColor = color*diCo;
		specularColor = color_RGBA(1.0, 1.0, 1.0, 1.0)*spCo;
		ambientCoefficient = amCo;
		diffuseCoefficient = diCo;
		specularCoefficient = spCo;
		specularPower = spPo;
		alpha = a;
	}

	surface(triple color, float aC, float dC, float sC, int sP, float a)
	{
		surface(color_RGBA(color), aC, dC, sC, sP, a);
	}


	~surface()
	{

	}

	float * getRGBColor()
	{
		return getAmColor();
	}

	float * getAmColor()
	{
		return ambientColor.getRGBColor();
	}

	float * getDiffColor()
	{
		return diffuseColor.getRGBColor();
	}


	float * getSpecColor()
	{
		return specularColor.getRGBColor();
	}

	int getSpecPower()
	{
		return specularPower;
	}
};


#endif