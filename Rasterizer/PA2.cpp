/* 
*
*	PA2.cpp
*   Created by Schuyler Kylstra
* 	10/6/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*	I found an example of using OpenGL and Glut online 
*	here: http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/sample.c
*	I built my program off of this example
*
*/

#include <stdio.h>
#include <math.h>
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

GLdouble Gwidth, Gheight;
int Gwd;                   /* GLUT window handle */

//////////////////////////////////////////////////////included////////////////////////////////////////////////////////

int     gNumVertices    = 0;    // Number of 3D vertices.
int     gNumTriangles   = 0;    // Number of triangles.
int*    gIndexBuffer    = NULL; // Vertex indices for the triangles.

LPoint 	lightLocation;
Surface	sphereColor;

template <typename T>
T interpolate(T t1, T t2, T t3, float beta, float gamma)
{
	if (beta + gamma > 1.0)
	{
		return NULL;
	}
	return t1 + ((t2 - t1)*beta) + ((t3 - t1)*gamma);
}




void create_scene()
{
    int width   = 32;
    int height  = 16;
    
    float theta, phi;
    int t;
    
    gNumVertices    = (height - 2) * width + 2;
    gNumTriangles   = (height - 2) * (width - 1) * 2;
    
    // TODO: Allocate an array for gNumVertices vertices.
    LPoint vertexArray[gNumVertices];


    gIndexBuffer    = new int[3*gNumTriangles];
    
    t = 0;
    for (int j = 1; j < height-1; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            theta = (float) j / (height-1) * M_PI;
            phi   = (float) i / (width-1)  * M_PI * 2;
            
            float   x   = sinf(theta) * cosf(phi);
            float   y   = cosf(theta);
            float   z   = -sinf(theta) * sinf(phi);
            
            // TODO: Set vertex t in the vertex array to {x, y, z}.
            vertexArray[t] = LPoint(x,y,z);
            
            t++;
        }
    }
    
    // TODO: Set vertex t in the vertex array to {0, 1, 0}.
    vertexArray[t] = LPoint(0,1,0);
    
    t++;
    
    // TODO: Set vertex t in the vertex array to {0, -1, 0}.
    vertexArray[t] = LPoint(0,-1,0);

    t++;
    
    t = 0;
    for (int j = 0; j < height-3; ++j)
    {
        for (int i = 0; i < width-1; ++i)
        {
            gIndexBuffer[t++] = j*width + i;
            gIndexBuffer[t++] = (j+1)*width + (i+1);
            gIndexBuffer[t++] = j*width + (i+1);
            gIndexBuffer[t++] = j*width + i;
            gIndexBuffer[t++] = (j+1)*width + i;
            gIndexBuffer[t++] = (j+1)*width + (i+1);
        }
    }
    for (int i = 0; i < width-1; ++i)
    {
        gIndexBuffer[t++] = (height-2)*width;
        gIndexBuffer[t++] = i;
        gIndexBuffer[t++] = i + 1;
        gIndexBuffer[t++] = (height-2)*width + 1;
        gIndexBuffer[t++] = (height-3)*width + (i+1);
        gIndexBuffer[t++] = (height-3)*width + i;
    }
    
    // The index buffer has now been generated. Here's how to use to determine
    // the vertices of a triangle. Suppose you want to determine the vertices
    // of triangle i, with 0 <= i < gNumTriangles. Define:
    //
    // k0 = gIndexBuffer[3*i + 0]
    // k1 = gIndexBuffer[3*i + 1]
    // k2 = gIndexBuffer[3*i + 2]
    //
    // Now, the vertices of triangle i are at positions k0, k1, and k2 (in that
    // order) in the vertex array (which you should allocate yourself at line
    // 27).
    //
    // Note that this assumes 0-based indexing of arrays (as used in C/C++,
    // Java, etc.) If your language uses 1-based indexing, you will have to
    // add 1 to k0, k1, and k2.

	/*BEGIN PIPELINE*/




    //Modelling Transform
	//We are given a unit sphere at the origin but we want a sphere of radius 2 at (0,0,-7)

	//scale by 2 in every direction THEN move by (0,0,-7) so: Mt*Ms



    //Camera Transform




    //Projection Transform




    //Viewport Transform










}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





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

	s1 = sphere(tuple(-4., 0., -7.), 1., Surface(color_RGBA(1.0, 0.0, 0.0), .2, 1.f, 0.0, 0.0));
	s2 = sphere(tuple(0., 0., -7.), 2., Surface(color_RGBA(0.0, 1.0, 0.0), .2, .5, .5, 32));
	s3 = sphere(tuple(4., 0., -7.), 1., Surface(color_RGBA(0.0, 0.0, 1.0), .2, 1.0, 0.0, 0.0));
	p1 = plane(tuple(0.0, -2., -7.0), spaceVector(0.0,1.0, 0.0), Surface());

	PARTS[0] = &s1;
	PARTS[1] = &s2;
	PARTS[2] = &s3;
	PARTS[3] = &p1;

	light0 = lightSource(color_RGBA(1.0, 1.0, 1.0, 1.0), tuple(-4., 4., -3.));
	LIGHTS[0] = &light0;
}



/* Draw the window - this is where all the GL actions are */
void display(void) ///DEPRICATED - I switched the production function to create_scene
{
	/* init GL */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	/* clear the screen to black */
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	glColor3f(0.0, 0.0, 0.0);
	glColor3fv((GLfloat *) cam.calcColorVal(PARTS, p, alias, &normRay, &temp));
	glVertex2i(i, j);

	glEnd();
	glFlush();

	return;
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
	Gwd = glutCreateWindow("Rasterizer" /* title */ );

	/* --- register callbacks with GLUT --- */

	/* register function to handle window resizes */
	glutReshapeFunc(reshape);

	/* register keyboard event processing function */
	glutKeyboardFunc(kbd);

	/* register function that draws in the window */
	glutDisplayFunc(create_scene);


	/* set the size of the verticies to 1 pixel */
	glPointSize(1);

	/* start the GLUT main loop */
	glutMainLoop();

	exit(0);
}