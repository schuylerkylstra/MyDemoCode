Schuyler Kylstra
COMP 770
Programming Assignment 4
Dinesh Manoucha
10/26/14


INTRODUCTION:
	This is the PA4 submission for Schuyler Kylstra for Comp 770 Fall 2014. It is an implementation of
	A raytracer with reflections and tree traversal over a kdTree. 



COMPILATION:
	The makefile has been supplied. Simply run make in the terminal in the program's folder. This
	will make 2 different compilations. one that is a debug compatible make and the other which is
	optimized for speed. To make only the speed optimized version type: "make full" into the
	terminal.



RUNNING:
	To run the program simply type ./PA4 into the command line. This will render Part1 of the assignment
	
	Render options:
		Spheres:  	
				$ ./PA4   
			or
				$ ./PA4 SPHERE
		Cathedral:
			no shadows:
				$ ./PA4 CAT
			shadows:
				$ ./PA4 SHAD


BREAKDOWN:

	PA4.cpp:
		I built this file around an OpenGL example I found online that I also used in PA1, 2, and 3.
		It comes from the website: http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/sample.c
		This file initializes the OpenGL window as well as the scene objects
		This file contains the main method.

	structures.h
		This file only contains #include statements for the other structs I use in the assignment. It includes 
		all the header files except for camera.h

	camera.h
		Contains the workings of the camera class. I overengineered this class and implemented some of the 
		methods elsewhere too. Mostly just used to calculate the color returned from a given ray but only
		in PART1.

	header files
		contain classes/structs that I use in the program. 

	display.cpp
		This file contains the display functions that I pass to OpenGl as well as two functions to shoot rays.
		Both of the ray shooting functions are recursive, the difference is that one recurses over a kdTree 
		while the other recurses over reflections. The reflection only recurses 10 layers down.

	intersect.cpp
		This file contains most of the code that evaluates a ray intersection with a kdNode volume. 

	load_mesh.cpp
		This is the mesh-loading code supplied supplimented by my kdTree parser. I also slightly modified the 
		Vector3 definition by adding opperator methods. Additionally, I added a KDNode struct in this file.

	shade.cpp
		This is the file I use to hold the different methods to color in the KDtree. Both the shadowed and 
		un-shadowed versions are here. They are called using the pointer function colorize(...) which is 
		initialized in PA4.cpp. 

	TRIntersect.cpp
		This is the very bottom of KDtree chain. This file contains the Moller-Trumbore algorithm to efficiently 
		determine whether or not a ray intersects a triangle and returns the betta and gamma values as well as
		the time of impact. I took this code almost directly from the wikipedia page on the Moller-Trumbore alg:
		http://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
