/* 
*
*	camera.h
*   Created by Schuyler Kylstra
* 	9/9/14
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

	tuple location;

	lightSource * LIGHTS[];

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
			
			spaceVector lDir = (loc - (LIGHTS[i]->getLocation() ) );
			float distToLight = lDir.getMag();
			lDir.normalize();
			ray lRay = ray(LIGHTS[i]->getLocation(), lDir, 0.0, distToLight); // we offset in the direction of the normal by .001


			//	here we need to check to see if I = 0 or 1
			seenPair illuminated = chooseSeenObj(PARTS, 4, &lRay);

			if(illuminated.index == 4 || illuminated.index == pair.index)
			{
				
				finalColor + ((color->diffuseColor)*fmax(0.0, lDir.dotProduct(normal)*-1));

				if ((color->specularCoefficient) != 0.0)
				{
					spaceVector bisector = (lDir + dir)*(-1); // they are both normalized vectors so the difference is the bisector (because lDir goes from impact to light)
					bisector.normalize();
					float coeff = fmax(0.0, bisector.dotProduct(normal));

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
		// finalColor.cap();
		return finalColor;
	}






	color_RGBA antiAliased(visible *PARTS[], seenPair pair, ray * normRay, ray * originalRay)
	{
		// half pixel length = .000195
		color_RGBA finalColor = aliased(PARTS, pair, normRay);
		seenPair randPair;
		ray randRay;
		color_RGBA added;
		
		float dX;
		float dY;
		
		
		for (int i = 0; i < 63; ++i)
		{
			// dX = -0.0001953125 + ((float) (rand())/((float) RAND_MAX)) * .000390625;
			dX = 2*-.000390625 + ((float) (rand())/((float) RAND_MAX)) * .00078125*2;
			dY = 2*-.000390625 + ((float) (rand())/((float) RAND_MAX)) * .00078125*2;
			// dY = -0.0001953125 + ((float) (rand())/((float) RAND_MAX)) * .000390625;

			randRay = originalRay->changeDirection(spaceVector(dX, dY, 0.0));

			randRay.normalize();

			randPair = chooseSeenObj(PARTS, 4, &randRay);

			added = aliased(PARTS, pair, &randRay);

			finalColor + added;	
		}
		finalColor = finalColor.scalarMult(.015625);

		return finalColor;
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

		location = tuple(0.0, 0.0, 0.0);

		distBetweenPixelsX();
		distBetweenPixelsY();
	}



	camera(tuple loc, spaceVector viewDir, spaceVector upDir, float imageX, float imageY, int Xpix, int Ypix, float focalL)
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



	camera(lightSource * l[], int numLights)
	{
		focalLength = 0.1;

		xResolution = 512;
		yResolution = 512;

		imagePlaneXLength = .2;
		imagePlaneYLength = .2;

		viewNormal = spaceVector(0.0, 0.0, -1.0);
		imageYOrient= spaceVector(0.0, 1.0, 0.0);
		imageXOrient = spaceVector(1.0, 0.0, 0.0);

		location = tuple(0.0, 0.0, 0.0);

		distBetweenPixelsX();
		distBetweenPixelsY();

		for (int i = 0; i < numLights; ++i)
		{
			LIGHTS[i] = l[i];
		}

		numberLights = numLights;
	}


	seenPair chooseSeenObj(visible * shapes[], int size, int i, int j) // this function will choose the correct object that the camera sees for pixel[i, j]
	{

		float minT = FLT_MAX;
		int minIndex = size;
		float tempT;
		ray r = shootRay(i,j);

		for (int k = 0; k < size; ++k)
		{
			tempT = shapes[k]->intersectionTime(&r);
			if(tempT < minT)
			{
				minT = tempT;
				minIndex = i;
			}
		}
		return seenPair(minIndex, minT);
	}


	/** 
	*	This function used when the ray is determined external
	*	to the camera. This is useful because it can be much faster.
	*	Instead of recalculating the ray each time just update it
	*	using vector addition. This means there should be camera 
	*	info used outside of the camera
	**/
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


	float * calcColorVal(visible * PARTS[], seenPair pair, bool alias, ray * normRay, ray * origRay)
	{
		color_RGBA c;
		if(alias == 1)
		{
			c = aliased(PARTS, pair, normRay);
		}
		else
		{
			c = antiAliased(PARTS, pair, normRay, origRay);
		}

		c.gammaCorrect(.454545);
		return c.getRGBColor();
	}
};


#endif