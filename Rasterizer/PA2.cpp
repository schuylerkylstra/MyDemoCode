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
#include "Matrix.h"
#include "Triangle.h"
#include "Point.h"
#include "Surface.h"
#include "Vector.h"


#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLdouble Gwidth, Gheight;
int Gwd;                   /* GLUT window handle */

//////////////////////////////////////////////////////included////////////////////////////////////////////////////////

int     gNumVertices    = 0;    // Number of 3D verticies.
int     gNumTriangles   = 0;    // Number of triangles.
int*    gIndexBuffer    = NULL; // Vertex indices for the triangles.

uint8_t shader			= 0x0;	// Number to determine flat, Gauraurd, or Phong shading (0, 1, or 2);

uint8_t FLAT 			= 0x0;	// 0000 0000
uint8_t GAURAURD 		= 0x1;	// 0000 0001
uint8_t PHONG 			= 0x3;	// 0000 0011

LPoint 	lightLocation;
LPoint	camera;
Surface	sphereColor;



template <typename T>
T interpolate(T t1, T t2, T t3, float beta, float gamma)
{
	return t1 + ((t2 - t1)*beta) + ((t3 - t1)*gamma);
}



float * flatColorize(Triangle T, float betta, float gamma)
{
	return T.getColor().getRGBColor();
}



float * gaurColorize(Triangle T, float betta, float gamma)
{
	LVertex ** verts = T.getVerts();
	RGB_Color c = interpolate(	(*verts[0]).getColor(), 
								(*verts[1]).getColor(),
								(*verts[2]).getColor(),
								betta, gamma);
	c.gammaCorrect();
	return c.getRGBColor();
}



float * phongColorize(Triangle T, float betta, float gamma)
{
	LVertex ** verts = T.getVerts();
	LPoint p1 = (*verts[0]).getLoc();
	LPoint p2 = (*verts[1]).getLoc();
	LPoint p3 = (*verts[2]).getLoc();

	LVector v1 = LVector(p1.coords[0],p1.coords[1],p1.coords[2]);
	LVector v2 = LVector(p2.coords[0],p2.coords[1],p2.coords[2]);
	LVector v3 = LVector(p3.coords[0],p3.coords[1],p3.coords[2]);

	LVector n = interpolate(	(*verts[0]).getNormal(),
								(*verts[1]).getNormal(),
								(*verts[2]).getNormal(),
								betta, gamma);
	n.normalize();

	LVector p = interpolate(	v1,
								v2,
								v3,
								betta, gamma);
	LPoint location = LPoint(p.coords[0], p.coords[1], p.coords[2]);


	LVector l = lightLocation - location;
	LVector v = camera - location;
	l.normalize();
	v.normalize();
	LVector h = (l + v).unitVect();
	RGB_Color c = sphereColor.getRGBColor(l.dotProduct(&n), h.dotProduct(&n));
	c.gammaCorrect();
	return c.getRGBColor();
}





void create_scene(void)
{
	float depthBuffer[262144];
	std::fill(depthBuffer,depthBuffer+262144,-1000.0); // set all of the values in depthBuffer to floating point -1000.0
    int width   = 32;
    int height  = 16;
    
    float theta, phi;
    int t;
    
    gNumVertices    = (height - 2) * width + 2;
    gNumTriangles   = (height - 2) * (width - 1) * 2;
    
    LPoint * pointArray = new LPoint[gNumVertices];
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
            
            pointArray[t] = LPoint(x,y,z);
            t++;
        }
    }

    LPoint Ptop = LPoint(0.0, 1.0, 0.0);
    pointArray[t] = Ptop;
    t++;
    

    LPoint Pbot = LPoint(0.0, -1.0, 0.0);
    pointArray[t] = Pbot;
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
    // the verticies of a triangle. Suppose you want to determine the verticies
    // of triangle i, with 0 <= i < gNumTriangles. Define:
    //
    // k0 = gIndexBuffer[3*i + 0]
    // k1 = gIndexBuffer[3*i + 1]
    // k2 = gIndexBuffer[3*i + 2]
    //
    // Now, the verticies of triangle i are at positions k0, k1, and k2 (in that
    // order) in the vertex array (which you should allocate yourself at line
    // 27).
    //
    // Note that this assumes 0-based indexing of arrays (as used in C/C++,
    // Java, etc.) If your language uses 1-based indexing, you will have to
    // add 1 to k0, k1, and k2.


	Triangle * faces = new Triangle[gNumTriangles];
	LVertex * verticies = new LVertex[gNumVertices];
	
	/********************BEGIN PIPELINE********************/

    /*****Modelling Transform*****/
	LMatrix scale = LMatrix();
	scale.makeScale(2.0,2.0,2.0);
	LMatrix mod = LMatrix();
	mod.makeTranslate(0.0, 0.0, -7.0);
	mod = mod.matrixMult(scale);
	/******************************/


	for (int i = 0; i < gNumVertices; ++i)
	{
		mod.pointMult(&pointArray[i]);
	}


	// Make the triangles
	if(shader == FLAT)
	{
		for (int i = 0; i < gNumTriangles; ++i)
		{
			int pos = 3*i;
			int k0 = gIndexBuffer[pos];
			int k1 = gIndexBuffer[pos + 1];
			int k2 = gIndexBuffer[pos + 2];
			faces[i] = Triangle(&pointArray[k0], &pointArray[k1], &pointArray[k2]);
			faces[i].colorize(lightLocation, camera, sphereColor);
		}
	}
	else // BOTH GAUR SHADING AND PHONG SHADING NEED THE VERTEX NORMALS
	{
		for (int i = 0; i < gNumVertices; ++i)
		{
			verticies[i] = LVertex(&pointArray[i]);
		}

		for (int j = 0; j < gNumTriangles; ++j)
		{
			int pos = 3*j;
			int k0 = gIndexBuffer[pos];
			int k1 = gIndexBuffer[pos + 1];
			int k2 = gIndexBuffer[pos + 2];
			faces[j] = Triangle(&verticies[k0], &verticies[k1], &verticies[k2]);
		}

		if(shader == GAURAURD)
		{
			for (int i = 0; i < gNumTriangles; ++i)
			{
				faces[i].colorizeVerts(lightLocation, camera, sphereColor);
			}
		}
	}




    /*****Camera Transform*****/
	float a[] = {1.0, 0.0, 0.0, 0.0};
	float b[] = {0.0, 1.0, 0.0, 0.0};
	float c[] = {0.0, 0.0, 1.0, 0.0};
	float d[] = {0.0, 0.0, 0.0, 1.0};
    LMatrix mCam = LMatrix(a,b,c,d); /// Not sure if it should have a negated Z or not
    /******************************/

    /*****Projection Transform****/
    LMatrix pers = LMatrix(); //convert to a perspective
    pers.makePerspective(-.1, -1000.f);
    LMatrix M1 = LMatrix();
    LMatrix Mor = LMatrix(); //3D orthogonal projection
    M1.makeScale(10., 10., 2./999.9);
    Mor.makeTranslate(0.0, 0.0, 1000.1/999.9);
    Mor = Mor.matrixMult(M1);
    /******************************/

    /*****Viewport Transform****/
    LMatrix Mvp = LMatrix(Gwidth, Gheight);
    /******************************/


    Mvp = Mvp.matrixMult(Mor.matrixMult(pers.matrixMult(mCam)));	// Final MATRIX!!!

	
	float invW;
    for (int i = 0; i < gNumVertices; ++i)
    {
    	Mvp.pointMult(&pointArray[i]);
    	invW = (1.0/(pointArray[i].coords[3]));
    	pointArray[i].scalarMult(invW); // normalize the w coordinate
    }

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    /******************************/
    //	We are now ready to write to the screen
    /******************************/

    // Colorize depending ont the shader
    float * (*returnColor)	(Triangle, float, float);

    if(shader == FLAT)
    {
    	returnColor = &flatColorize;
    }
    else if (shader == GAURAURD)
    {
    	returnColor = &gaurColorize;
    }
    else
    {
    	returnColor = &phongColorize;
    }

    //For each triangle
    CtoFMat interpMat; // Canonical to frame matrix
	int xMin, xMax;
	int yMin, yMax;
	float bettaGamma[3];
	float depth;
	int bufIndex;

	glBegin(GL_POINTS);
    for (int i = 0; i < gNumTriangles; ++i)
    {
		int pos = 3*i;
		int k0 = gIndexBuffer[pos];
		int k1 = gIndexBuffer[pos + 1];
		int k2 = gIndexBuffer[pos + 2];
		LPoint p0, p1, p2;
		p0 = pointArray[k0];
		p1 = pointArray[k1];
		p2 = pointArray[k2];

		interpMat = CtoFMat(p0, p1, p2);

		xMin = (int) floorf(fminf(p0.coords[0],fminf(p1.coords[0],p2.coords[0])));
		xMax = (int) floorf(fmaxf(p0.coords[0],fmaxf(p1.coords[0],p2.coords[0])));
		yMin = (int) floorf(fminf(p0.coords[1],fminf(p1.coords[1],p2.coords[1])));
		yMax = (int) floorf(fmaxf(p0.coords[1],fmaxf(p1.coords[1],p2.coords[1])));

		for (int y = yMin; y <= yMax; ++y)
		{
			for (int x = xMin; x <= xMax; ++x)
			{
				interpMat.bG(x, y, bettaGamma); //This is inefficient. Should alter to incremental algorithm


				if(bettaGamma[0] >= 0 & bettaGamma[1] >= 0 & bettaGamma[2] <= 1)
				{
					depth = interpolate(p0.coords[2], p1.coords[2], p2.coords[2], bettaGamma[0], bettaGamma[1]);
					bufIndex = (x + y*512);
					if(depth > depthBuffer[bufIndex])
					{
						depthBuffer[bufIndex] = depth;
    					glColor3fv((GLfloat *) returnColor(faces[i], bettaGamma[0], bettaGamma[1]));
    					glVertex2i(x, y);
    				}
				}
			}
		}
    }


    glEnd();
    glFlush();
    delete[] pointArray;
    delete[] faces;
    delete[] verticies;
    return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void init(char * init[], int argc)
{

	std::string str1 ("FLAT");
	std::string str2 ("GAUR");
	std::string str3 ("PHONG");

	// alias = 0;						/* initialize the system to alias */
	if(argc > 1)
	{	
		if (str1.compare(init[1]) == 0)
		{
			shader = FLAT;
		}
		else if (str2.compare(init[1]) == 0)
		{
			shader = GAURAURD;
		}
		else
		{
			shader = PHONG;
		}
	}

	/* (0,0) is the lower left corner */
	RGB_Color amC = RGB_Color(0.0, 0.2, 0.0);
	RGB_Color diC = RGB_Color(0.0, 0.5, 0.0);
	RGB_Color spC = RGB_Color(0.5, 0.5, 0.5);
	sphereColor = Surface(amC, diC, spC, 32);
	camera = LPoint(0.0, 0.0, 0.0);
	lightLocation = LPoint(-4.0, 4.0, -3.0);

	Gwidth  = 512.0;				/* initial window Gwidth and Gheight, */
	Gheight = 512.0;				/* within which we draw. */
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
	  		// switchAliasing();

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