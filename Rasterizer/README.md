Schuyler Kylstra
COMP 770
Programming Assignment 2
Dinesh Manoucha
9/22/14


INTRODUCTION:
	This is the PA2 submission for Schuyler Kylstra for Comp 770 Fall 2014. It is a rasterizer
	that can swith between flat, Gauraurd, and Phong shading. It combines ambient light with
	specular and diffuse lighting to calculate the color at a given pixel.



COMPILATION:
	The makefile has been supplied. Simply run make in the terminal in the program's folder. This
	will make 2 different compilations. one that is a debug compatible make and the other which is
	optimized for speed. To make only the speed optimized version type: "make full" into the
	terminal.



RUNNING:
	To run the program simply type ./PA2 into the command line. This will render via flat shading.
	
	Shading options:
		Flat:  	
				$ ./PA2   
			or
				$ ./PA2 FLAT
		Gauraurd:
				$ ./PA2 GAUR
		Phong:
				$ ./PA2 PHONG


BREAKDOWN:

	PA2.cpp:
		I built this file around an OpenGL example I found online that I also used in PA1.
		It comes from the website: http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/sample.c
		This file initializes the OpenGL window and is where all of the general environment
		objects get instantiated. This is the file where the points for the sphere are generated.
		All of the shaders are small functions inside this file. Additionally, I put a function 
		template to interpolate values given 3 different inputs. 
		This file contains the main method.

	Triangle.h
		This file contains the Triangle class. It can be composed from 3 points or 3 verticies,
		depending on the type of shading. The difference between the two is that the verticies
		contain a normal vector that represents the average normal of all the triangles that contain
		that vertex. This is used in Phong and Gauraurd shading. 

	Matrix.h
		This file contains all the math for matrix transformations.

	Point.h
		This file contains the LPoint and LVertex classes. LPoint is more general than LVertex and 
		Point arithmatic results in LVectors.

	Vector.h
		This file contains the LVector class which is like the arithmetic vector in 3 dimensions.
		However, it has 4 floats becasue it is in homogeneous coordinate space.

	Surface.h
		This file contains the RGB_Color and Surface classes. The RGB_Color class is an array of 3 floats
		while the Surface class takes 3 different RGB_Color objects and an int to represent ambient, specular,
		and diffuse lighting.

	

