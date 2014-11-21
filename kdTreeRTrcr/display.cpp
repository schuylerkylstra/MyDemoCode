/* 
*
*	display.cpp
*   Created by Schuyler Kylstra
* 	11/12/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/




#ifndef disp_cpp
#define disp_cpp

#include "visible.h"
#include "structures.h"
#include "intersect.cpp"
#include "shade.cpp"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLdouble Gwidth, Gheight;
int 	Gwd;                   /* GLUT window handle */
bool 	alias;
bool 	intersections;



visible * 		PARTS[4];
lightSource * 	LIGHTS[1];

sphere 	s1;
sphere 	s2;
sphere 	s3;
plane 	p1;

surface triColor;

lightSource light0;
camera 		cam;


void reflectRay(const seenPair pair, const ray r, ray& ref)
{
	visible * hit 		= PARTS[pair.index];
	spaceVector dir 	= r.direction;
	spaceVector loc 	= ((dir*(pair.T)) + (r.origin)); // this places the location of the intersection .001 above the surface
	spaceVector normal 	= hit->getNormal(loc);

	loc = loc + normal*.001;
	dir = dir - (normal * ((dir.dotProduct(normal)) * 2.0));

	triple l = triple(loc.X, loc.Y, loc.Z);

	ref.origin = l;
	ref.direction = dir;
	ref.tMin = 0.f;
	ref.tMax = FLT_MAX;

	ref.normalize();
}




void tShootRay(const ray& r, float color[3])
{
	color[0] = 1.f;
	color[1] = 0.f;
	color[2] = 0.f;

	float BG[2] = {0};

	Vector3	Origin  = {r.origin.X,		r.origin.Y, 	r.origin.Z};
	Vector3 Dir 	= {r.direction.X, 	r.direction.Y,  r.direction.Z};

	int index 	= -1;
	float t 	= FLT_MAX;

	if(rIntersection(Origin, Dir, gKDTree[0], index, t, BG))
	{
		colorize(Origin, Dir, index, t, BG, color);	
	}
}



void rShootRay(ray & r, float localColor[3], int depth)
{
	seenPair pair = cam.chooseSeenObj(PARTS, 4, &r);

	if (pair.index == 4 || depth > 10)
	{
		localColor[0] = 0.f;
		localColor[1] = 0.f;
		localColor[2] = 0.f;
	}
	else
	{
		float * temp = cam.calcColorVal(PARTS, pair, &r);

		localColor[0] = temp[0];
		localColor[1] = temp[1];
		localColor[2] = temp[2];

		float a = PARTS[pair.index]->getAlpha();

		if (a > 0)
		{
			float * refColor	= new float[3];
			ray reflected = ray();
			reflectRay(pair, r, reflected);
			rShootRay(reflected, refColor, depth + 1); 

			localColor[0] = localColor[0] * (1-a) + refColor[0] * a;
			localColor[1] = localColor[1] * (1-a) + refColor[1] * a;
			localColor[2] = localColor[2] * (1-a) + refColor[2] * a;

			delete[] refColor;
		}
	}	
}



void gammaCorrect(float* color, float c)
{
	color[0] = pow(color[0], c);
	color[1] = pow(color[1], c);
	color[2] = pow(color[2], c);
}


/* Draw the window - this is where all the GL actions are */
void displaySpheres(void)
{
	/* init GL */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	/* clear the screen to black */
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);


	float A = (-.2 + .2/512.f)/2.f;
	ray r = ray(triple(0.0, 0.0, 0.0), spaceVector(A, A, -.1), 0.f, FLT_MAX);

	spaceVector deltaX = spaceVector(.2/512.0, 0, 0);
	spaceVector deltaY = spaceVector(0.0, .2/512.0, 0.0);

	ray 		temp;
	ray 		normRay;
	seenPair 	pair;
	float * 	color = new float[3];

	for (int j = 0; j < 512; j++) 
	{
		temp = r;

		for (int i = 0; i < 512; i++) 
		{
			normRay = temp.normalized();


			rShootRay(normRay, color, 0);
			gammaCorrect(color, .454545);
			glColor3fv((GLfloat *) color);
			glVertex2i(i, j);


			temp = temp.changeDirection(deltaX);
		}

		r = r.changeDirection(deltaY);
	}

	delete[] color;
	glEnd();
	glFlush();
	return;
}



void displayCathedral(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	/* clear the screen to black */
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);


	float A = (-.2 + .2/512.f)/2.f;


	ray r = ray(triple(0.f, -10.f, 0.f), spaceVector(.1, A, A), 0.f, FLT_MAX);

	spaceVector deltaZ = spaceVector(0.f, 0.f, .2/512.0);
	spaceVector deltaY = spaceVector(0.f, .2/512.0, 0.f);

	ray 		temp;
	ray 		normRay;
	seenPair 	pair;
	float * 	color = new float[3];
	

	for (int j = 0; j < 512; j++) 
	{
		temp = r;
		gJ = j;
		for (int i = 0; i < 512; i++) 
		{
			gI = i;
			normRay = temp.normalized();

			tShootRay(normRay, color);
			gammaCorrect(color, .454545);
			glColor3fv((GLfloat *) color);
			glVertex2i(i, j);


			temp = temp.changeDirection(deltaZ);
		}

		r = r.changeDirection(deltaY);
	}

	delete[] color;
	glEnd();
	glFlush();
	return;
}


#endif