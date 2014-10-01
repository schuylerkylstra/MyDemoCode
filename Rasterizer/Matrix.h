/* 
*
*	Matrix.h
*   Created by Schuyler Kylstra
* 	10/6/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef TRNS_LMATRIX_H
#define TRNS_LMATRIX_H

#include "Point.h"
#include "Vector.h"
#include <math.h>

#ifndef degToRad
#define degToRad 0.01745329
#endif

#ifndef POINT_W
#define POINT_W 1
#endif

#ifndef VECT_W
#define VECT_W 0
#endif

#ifndef ROW
#define ROW {0.0, 0.0, 0.0, 1.0}
#endif

class LMatrix
{
	LMatrix()
	{
		a[0] = 1;
		b[1] = 1;
		c[2] = 1;
	}

	LMatrix(float iA[4], float iB[4], float iC[4])
	{
		for (int i = 0; i < 4; ++i)
		{
			a[i] = iA[i];
			b[i] = iB[i];
			c[i] = iC[i];
		}
	}


	~LMatrix(){}

	void pointMult(LPoint *);

	void vectorMult(LVector *);

	LMatrix matrixMult(LMatrix);

	LMatrix tanspose();

	void makeIdent();

	void makeTranslate(float, float, float);

	void makeScale(float, float, float);

	void makeXRotate(float, float, float);

	void makeYRotate(float, float, float);

	void makeZRotate(float, float, float);


	float a[4]
	float b[4];
	float c[4];

};


///////////////////////////////////////////////////////////////////////////////


LMatrix LMatrix::matrixMult(LMatrix M)
{
	float ia[4];
	float ib[4]; 
	float ic[4];

	for (int i = 0; i < 4; ++i)
	{
		ia[i] = a[0]*M.a[i] + a[1]*M.b[i] + a[2]*M.c[i] + a[3]*ROW[i];
		ib[i] = b[0]*M.a[i] + b[1]*M.b[i] + b[2]*M.c[i] + b[3]*ROW[i];
		ic[i] = c[0]*M.a[i] + c[1]*M.b[i] + c[2]*M.c[i] + c[3]*ROW[i];
	}
	return LMatrix(ia, ib, ic);
}



void LMatrix::pointMult(LPoint * p)
{
	float * v = p->getCoords();
	float ix = a[0]*v[0] + a[1]*v[1]  + a[2]*v[2] + a[3]*POINT_W;
	float iy = b[0]*v[0] + b[1]*v[1]  + b[2]*v[2] + b[3]*POINT_W;
	float iz = c[0]*v[0] + c[1]*v[1]  + c[2]*v[2] + c[3]*POINT_W;
	p->setPosition(ix, iy, iz)
}



void LMatrix::vectorMult(LVector * vec)
{
	float * v = vec->getCoords();
	float ix = a[0]*v[0] + a[1]*v[1]  + a[2]*v[2] + a[3]*VECT_W;
	float iy = b[0]*v[0] + b[1]*v[1]  + b[2]*v[2] + b[3]*VECT_W;
	float iz = c[0]*v[0] + c[1]*v[1]  + c[2]*v[2] + c[3]*VECT_W;
	vec->setPosition(ix, iy, iz);
}



void LMatrix::makeIdent()
{
	newA [] = {1.0, 0.0, 0.0, 0.0};
	newB [] = {0.0, 1.0, 0.0, 0.0};
	newC [] = {0.0, 0.0, 1.0, 0.0};

	for (int i = 0; i < 4; ++i)
	{
		a[i] = newA[i];
		b[i] = newB[i];
		c[i] = newC[i];
	}
}



void LMatrix::makeTranslate(float tx, float ty, float tz)
{
	makeIdent();
	a[3] = tx;
	b[3] = ty;
	c[3] = tz; 
}



void LMatrix::makeScale(float sx, float sy, float sz)
{
	makeIdent();
	a[0] = sx;
	b[1] = sy;
	c[2] = sz;
}



void LMatrix::makeXRotate(float degree)
{
	makeIdent();
	float arg = degree*degToRad;
	float co = cos(arg);
	float si = sin(arg);
	b[1] = co;
	b[2] = -si;
	c[1] = si;
	c[2] = co;
}



void LMatrix::makeYRotate(float degree)
{
	makeIdent();
	float arg = degree*degToRad;
	float co = cos(arg);
	float si = sin(arg);
	a[0] = co;
	a[2] = -si;
	c[0] = si;
	c[2] = co;
}



void LMatrix::makeZRotate(float degree)
{
	makeIdent();
	float arg = degree*degToRad;
	float co = cos(arg);
	float si = sin(arg);
	a[0] = co;
	a[1] = -si;
	b[0] = si;
	b[1] = co;
}

#endif