/* 
*
*	camera.h
*   Created by Schuyler Kylstra
* 	11/12/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef ENV_camera_H
#define ENV_camera_H

#include "structures.h"
#include "visible.h"
#include "sphere.h"
#include "plane.h"
#include "lightSource.h"
#include <float.h>
#include <iostream>
#include <math.h>


class camera
{
private:
	int xResolution, yResolution;
	int numberLights;

	float imagePlaneYLength, imagePlaneXLength;
	float focalLength;
	float pixelDistX, pixelDistY;

	spaceVector viewNormal;
	spaceVector imageYOrient; // the direction that corresponds to the top of the screen. WE can calculate the left right vector from these two vectors
	spaceVector imageXOrient;

	triple location;

	lightSource light;

	void distBetweenPixelsX() //this formulation is very static. it prevents any amount of turning at all 
	{
		pixelDistX = imagePlaneXLength/(2*xResolution);
	}


	void distBetweenPixelsY()
	{
		pixelDistY = imagePlaneYLength/(2*yResolution);
	}


	ray shootRay(int i, int j)
	{
		float xStart = (-imagePlaneXLength + pixelDistX)/2;
		float yStart = (-imagePlaneYLength + pixelDistY)/2;

		/* this will need to get updated when I incorperate the "up" and "right" direction vectors */
		spaceVector dir = spaceVector(xStart + i*pixelDistX,yStart + j*pixelDistY,-.1);

		ray r = ray(location, dir, 0.f, 100.f);
		return r;
	}




	color_RGBA aliased(visible * PARTS[], seenPair pair, ray * normRay)
	{


		// L = La + Ld + Ls
		// La = Ka*Ia
		// Ld = Kd*I*max(0, n.l)
		// Ls = Ks*I*max(0, n.h)^p
		// I is ilumination light (either 1 or zero depending on illumination or not)
		// n is the normal of the surface
		// l is the ray to the light
		// h is normalized bisector ray of l and vision ray
	
		visible * hit = PARTS[pair.index];

		surface * color = hit->getSurface();
		color_RGBA finalColor= color->ambientColor;

		spaceVector dir = normRay->direction;
		spaceVector loc = ((dir*(pair.T)) + (normRay->origin)); // this places the location of the intersection .001 above the surface
		spaceVector normal = hit->getNormal(loc); //the normal at the hit position
		loc = loc + normal*.001;
		dir.normalize();


		for (int i = 0; i < numberLights; ++i)
		{
			
			spaceVector lDir = (loc - (light.getLocation() ) );
			float distToLight = lDir.getMag();
			lDir.normalize();
			ray lRay = ray(light.getLocation(), lDir, 0.0, distToLight); // we offset in the direction of the normal by .001


			//	here we need to check to see if I = 0 or 1
			seenPair illuminated = chooseSeenObj(PARTS, 4, &lRay);

			if(illuminated.index == 4 || illuminated.index == pair.index)
			{
				
				finalColor + ((color->diffuseColor)* std::max(0.f, lDir.dotProduct(normal)*-1));

				if ((color->specularCoefficient) != 0.0)
				{
					spaceVector bisector = (lDir + dir)*(-1); // they are both normalized vectors so the difference is the bisector (because lDir goes from impact to light)
					bisector.normalize();
					float coeff = std::max(0.f, bisector.dotProduct(normal));

					if(coeff > 0.0)
					{
						float mult = coeff;
						int specPow = color->specularPower;
						for (int i = 0; i < specPow; ++i)
						{
							coeff *= mult;
						}
					}

					finalColor + ((color->specularColor)*coeff); 
				}	
			}	
		}

		return finalColor.scalarMult(1.f - color->alpha);
	}



public:

	camera()
	{
		focalLength = 0.1;

		xResolution = 512;
		yResolution = 512;

		imagePlaneXLength = .2;
		imagePlaneYLength = .2;

		viewNormal = spaceVector(0.0, 0.0, -1.0);
		imageYOrient= spaceVector(0.0, 1.0, 0.0);
		imageXOrient = spaceVector(1.0, 0.0, 0.0);

		location = triple(0.0, 0.0, 0.0);

		distBetweenPixelsX();
		distBetweenPixelsY();
	}



	camera(triple loc, spaceVector viewDir, spaceVector upDir, float imageX, float imageY, int Xpix, int Ypix, float focalL)
	{
		location = loc;

		viewNormal = viewDir;
		imageYOrient = upDir;
		viewNormal.normalize();
		imageYOrient.normalize();
		imageXOrient = upDir.crossProduct(viewDir);

		imagePlaneXLength = imageX;
		imagePlaneYLength = imageY;

		xResolution = Xpix;
		yResolution = Ypix;
		focalLength = focalL;

		distBetweenPixelsX();
		distBetweenPixelsY();
	}



	camera(lightSource& l, int numLights)
	{
		focalLength = 0.1;

		xResolution = 512;
		yResolution = 512;

		imagePlaneXLength = .2;
		imagePlaneYLength = .2;

		viewNormal = spaceVector(0.0, 0.0, -1.0);
		imageYOrient= spaceVector(0.0, 1.0, 0.0);
		imageXOrient = spaceVector(1.0, 0.0, 0.0);

		location = triple(0.0, 0.0, 0.0);

		distBetweenPixelsX();
		distBetweenPixelsY();

		light = l;

		numberLights = numLights;
	}


	camera(lightSource& l, int numLights, triple loc)
	{
		focalLength = 0.1;

		xResolution = 512;
		yResolution = 512;

		imagePlaneXLength = .2;
		imagePlaneYLength = .2;

		viewNormal = spaceVector(0.0, 0.0, -1.0);
		imageYOrient= spaceVector(0.0, 1.0, 0.0);
		imageXOrient = spaceVector(1.0, 0.0, 0.0);

		location = loc;

		distBetweenPixelsX();
		distBetweenPixelsY();

		light = l;

		numberLights = numLights;
	}



	seenPair chooseSeenObj(visible * shapes[], int size, ray * r) 
	{

		float minT = r->tMax;
		int minIndex = size;
		float tempT;

		for (int i = 0; i < size; ++i)
		{
			tempT = shapes[i]->intersectionTime(r);

			if(tempT < minT && tempT >= r->tMin )
			{
				minT = tempT;
				minIndex = i;
			}
		}
		return seenPair(minIndex, minT);
	}



	float * calcColorVal(visible * PARTS[], seenPair pair, ray * normRay)
	{
		static float color[3];
		color_RGBA c;
		c = aliased(PARTS, pair, normRay);
		color[0] = c.RGBA[0];
		color[1] = c.RGBA[1];
		color[2] = c.RGBA[2];
		return color;
	}
};


#endif