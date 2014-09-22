/* 
*
*	PA1.cpp
*   Created by Schuyler Kylstra
* 	9/9/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*	I found an example of using OpenGL and Glut online 
*	here: http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/sample.c
*	I built my program off of this example
*
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include "camera.h"
#include "sphere.h"
#include "plane.h"
#include "lightSource.h"


#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define NENDS 2           /* number of end "points" to draw */

GLdouble Gwidth, Gheight;
int Gwd;                   /* GLUT window handle */
bool alias;
bool intersections;

visible * PARTS[4];
lightSource * LIGHTS[1];

sphere s1;
sphere s2;
sphere s3;

plane p1;

lightSource light0;







void init(char * init[])
{
	std::string str1 ("alias");
	std::string str2 ("inter");

	alias = 0;						/* initialize the system to alias */

	if (str2.compare(init[1]) == 0)
	{
		intersections = 1;
	}
	if (str1.compare(init[1]) == 0)
	{
		alias = 1;
	}
	/* (0,0) is the lower left corner */

	Gwidth  = 512.0;				/* initial window Gwidth and Gheight, */
	Gheight = 512.0;				/* within which we draw. */

	s1 = sphere(tuple(-4., 0., -7.), 1., surface(color_RGBA(1.0, 0.0, 0.0), .2, 1.f, 0.0, 0.0));
	s2 = sphere(tuple(0., 0., -7.), 2., surface(color_RGBA(0.0, 1.0, 0.0), .2, .5, .5, 32));
	s3 = sphere(tuple(4., 0., -7.), 1., surface(color_RGBA(0.0, 0.0, 1.0), .2, 1.0, 0.0, 0.0));
	p1 = plane(tuple(0.0, -2., -7.0), spaceVector(0.0,1.0, 0.0), surface());

	PARTS[0] = &s1;
	PARTS[1] = &s2;
	PARTS[2] = &s3;
	PARTS[3] = &p1;

	light0 = lightSource(color_RGBA(1.0, 1.0, 1.0, 1.0), tuple(-4., 4., -3.));
	LIGHTS[0] = &light0;
}



/* Draw the window - this is where all the GL actions are */
void display1(void)
{
	/* init GL */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	/* clear the screen to black */
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);


	float A = (-.2 + .2/512.f)/2.f;
	ray r = ray(tuple(0.0, 0.0, 0.0), spaceVector(A, A, -.1), 0.f, FLT_MAX); ///***change to make generic ****


	camera cam = camera(LIGHTS, 1);


	spaceVector deltaX = spaceVector(.2/512.0, 0, 0);
	spaceVector deltaY = spaceVector(0.0, .2/512.0, 0.0);

	ray temp;
	ray normRay;
	seenPair p;
	for (int j = 0; j < 512; j++) // walk to top
	{
		temp = r;
		for (int i = 0; i < 512; i++) // walk to right
		{
			normRay = temp;
			normRay.normalize();


			// May want to edit this so it returns a list of the objects that it hits in order to
			// use in the calColorVal function.
			p = cam.chooseSeenObj(PARTS, 4, &normRay);			 


			if(p.index == 4)
			{
				glColor3f(0.0, 0.0, 0.0);
			}
			else
			{
				glColor3fv((GLfloat *) cam.calcColorVal(PARTS, p, alias, &normRay, &temp));
			}
			glVertex2i(i, j);

			temp = temp.changeDirection(deltaX);
		}
		r = r.changeDirection(deltaY);
	}

	glEnd();
	glFlush();

	return;
}



void display0(void)
{
	/* init GL */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	/* clear the screen to black */
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);


	float A = (-.2 + .2/512.f)/2.f;
	ray r = ray(tuple(0.0, 0.0, 0.0), spaceVector(A, A, -.1), 0.f, FLT_MAX); ///***change to make generic ****


	camera cam = camera(LIGHTS, 1);


	spaceVector deltaX = spaceVector(.2/512.0, 0, 0);
	spaceVector deltaY = spaceVector(0.0, .2/512.0, 0.0);

	ray temp;
	ray normRay;
	seenPair p;
	for (int j = 0; j < 512; j++) // walk to top
	{
		temp = r;
		for (int i = 0; i < 512; i++) // walk to right
		{
			normRay = temp;
			normRay.normalize();


			// May want to edit this so it returns a list of the objects that it hits in order to
			// use in the calColorVal function.
			p = cam.chooseSeenObj(PARTS, 4, &normRay);			 


			if(p.index == 4)
			{
				glColor3f(0.0, 0.0, 0.0);
			}
			else
			{
				glColor3f(1.0, 1.0, 1.0);
			}

			glVertex2i(i, j);

			temp = temp.changeDirection(deltaX);
		}
		r = r.changeDirection(deltaY);
	}

	glEnd();
	glFlush();

	return;
}



void switchAliasing()
{
	alias = (alias + 1) % 2;
}



/* Called when window is resized,
   also when window is first created,
   before the first call to display(). */
void reshape(int w, int h)
{
	/* save new screen dimensions */
	Gwidth = (GLdouble) w;
	Gheight = (GLdouble) h;

	/* tell OpenGL to use the whole window for drawing */
	glViewport(0, 0, (GLsizei) Gwidth, (GLsizei) Gheight);

	/* do an orthographic parallel projection with the coordinate
	system set to first quadrant, limited by screen/window size */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, Gwidth, 0.0, Gheight, -1.f, 1.f); // the -1.f and 1.f terms give the wedge of space that is visible in the orthographic projection.

	return;
}



void kbd(unsigned char key, int x, int y)
{
	switch((char)key) 
	{
		case 'q':
			glutDestroyWindow(Gwd);
	 		exit(0);

		case 27:    /* ESC */
	    	glutDestroyWindow(Gwd);
	 		exit(0);

		case 'a':
	  		switchAliasing();



		default:
	    	break;
  	}

  	return;
}


int main(int argc, char *argv[])
{
	/* perform initialization NOT OpenGL/GLUT dependent,
	as we haven't created a GLUT window yet */
	init(argv);

	/* initialize GLUT, let it extract command-line 
	GLUT options that you may provide 
	- NOTE THE '&' BEFORE argc */
	glutInit(&argc, argv);

	/* specify the display to be single 
	buffered and color as RGBA values */
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

	/* set the initial window size */
	glutInitWindowSize((int) Gwidth, (int) Gheight);

	/* create the window and store the handle to it */
	Gwd = glutCreateWindow("3D Graphics" /* title */ );

	/* --- register callbacks with GLUT --- */

	/* register function to handle window resizes */
	glutReshapeFunc(reshape);

	/* register keyboard event processing function */
	glutKeyboardFunc(kbd);

	/* register function that draws in the window */
	if(intersections == 1)
	{
		glutDisplayFunc(display0);
	}
	else
	{
		glutDisplayFunc(display1);
	}

	/* set the size of the verticies to 1 pixel */
	glPointSize(1);

	/* start the GLUT main loop */
	glutMainLoop();

	exit(0);
}