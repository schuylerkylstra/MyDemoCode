/* 
*
*	PA4.cpp
*   Created by Schuyler Kylstra
* 	11/12/14
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
#include "loadMesh.cpp"
#include "display.cpp"


#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define NENDS 2           /* number of end "points" to draw */

bool spheres = true;

void printCat()
{
	for (int i = 0; i < gKDTree.size(); ++i)
	{
		printf("%d, %d\n", gKDTree[i].indices[0], gKDTree[i].indices[0]);
	}
}

void init(char * init[], int argc)
{
	std::string str1 ("SPHERE");
	std::string str2 ("CAT");
	std::string str3 ("SHAD");

	if (argc > 1)
	{
		if (str3.compare(init[1]) == 0)
		{
			spheres = false;
			colorize = &shadows;
		}
		if (str2.compare(init[1]) == 0)
		{
			spheres = false;
			colorize = &noShadows;
		}
		if (str1.compare(init[1]) == 0)
		{
			spheres = true;
		}
	}
	
	// spheres = false;
	// colorize = &noShadows;

	alias = 1;						/* initialize the system to alias */

	Gwidth  = 512.0;				/* initial window Gwidth and Gheight, */
	Gheight = 512.0;				/* within which we draw. */

	triple L0 	= 	triple(-4., 0., -7.);
	triple L1 	= 	triple(0., 0., -7.);
	triple L2 	= 	triple(4., 0., -7.);
	triple L3 	= 	triple(0.0, -2., -7.0);

	surface red 	=	surface(color_RGBA(1.0, 0.0, 0.0), .2, 1.f, 0.0, 0.0, 0.f);
	surface green 	=	surface(color_RGBA(0.0, 1.0, 0.0), .2, .5, .5, 32, 0.f);
	surface blue 	= 	surface(color_RGBA(0.0, 0.0, 1.0), .2, 1.0, 0.0, 0.0, 0.8);
	surface white 	= 	surface(0.5);
	triColor	 	= 	surface(color_RGBA(1.0, 1.0, 1.0), 0.f, 1.f, 0.f, 0, 0.f);

	s1 	= 	sphere(L0, 1., red);
	s2 	= 	sphere(L1, 2., green);
	s3 	= 	sphere(L2, 1., blue);
	p1 	=	plane(L3, spaceVector(0.0, 1.0, 0.0), white);

	PARTS[0] = &s1;
	PARTS[1] = &s2;
	PARTS[2] = &s3;
	PARTS[3] = &p1;

	triple pos = triple(-4., 4., -3);

	if (!spheres)
	{
		pos = triple(0.f, 0.f, 0.f);	
	}

	light0 		=	lightSource(color_RGBA(1.0, 1.0, 1.0, 1.0), pos);
	LIGHTS[0] 	= 	&light0;

	cam = camera(light0, 1);

	if (!spheres)
	{
		cam = camera(light0, 1, triple(0.f, -10.0, 0.f));
	}
}



/* Called when window is resized,
   also when window is first created,
   before the first call to display(). */
void reshape(int w, int h)
{
	/* save new screen dimensions */
	Gwidth 	= (GLdouble) w;
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

		default:
	    	break;
  	}

  	return;
}



int main(int argc, char *argv[])
{
	/* perform initialization NOT OpenGL/GLUT dependent,
	as we haven't created a GLUT window yet */
	init(argv, argc);

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

	load_mesh("sibenik");

	load_tree("kdtree");

	/* register function to handle window resizes */
	glutReshapeFunc(reshape);

	/* register keyboard event processing function */
	glutKeyboardFunc(kbd);

	/* register function that draws in the window */
	if (spheres)
	{
		glutDisplayFunc(displaySpheres);
	}
	else
	{
		glutDisplayFunc(displayCathedral);
		// printCat();
	}

	/* set the size of the verticies to 1 pixel */
	glPointSize(1);


	/* start the GLUT main loop */
	glutMainLoop();

	exit(0);
}