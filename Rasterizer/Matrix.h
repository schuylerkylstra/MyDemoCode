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


class LMatrix
{
public:
	LMatrix()
	{
		makeIdent();
	}

	LMatrix(float iA[4], float iB[4], float iC[4], float iD[4])
	{
		for (int i = 0; i < 4; ++i)
		{
			a[i] = iA[i];
			b[i] = iB[i];
			c[i] = iC[i];
			d[i] = iD[i];
		}
		
	}

	LMatrix(float nX, float nY)
	{
		makeIdent();
		a[0] = nX/2.0;
		a[3] = (nX - 1.)/2.0;
		b[1] = nY/2.0;
		b[3] = (nY - 1.0)/2.0;

	}

	~LMatrix(){}

	void pointMult(LPoint *);

	void vectorMult(LVector *);

	LMatrix matrixMult(LMatrix);

	LMatrix tanspose();

	void makeIdent();

	void makeTranslate(float, float, float);

	void makeScale(float, float, float);

	void makeXRotate(float);

	void makeYRotate(float);

	void makeZRotate(float);

	void makePerspective(float, float);


	float a[4];
	float b[4];
	float c[4];
	float d[4];

};


///////////////////////////////////////////////////////////////////////////////


LMatrix LMatrix::matrixMult(LMatrix M)
{
	float ia[4];
	float ib[4]; 
	float ic[4];
	float id[4];

	for (int i = 0; i < 4; ++i)
	{
		ia[i] = a[0]*M.a[i] + a[1]*M.b[i] + a[2]*M.c[i] + a[3]*M.d[i];
		ib[i] = b[0]*M.a[i] + b[1]*M.b[i] + b[2]*M.c[i] + b[3]*M.d[i];
		ic[i] = c[0]*M.a[i] + c[1]*M.b[i] + c[2]*M.c[i] + c[3]*M.d[i];
		id[i] = d[0]*M.a[i] + d[1]*M.b[i] + d[2]*M.c[i] + d[3]*M.d[i];
	}
	return LMatrix(ia, ib, ic, id);
}


void LMatrix::makePerspective(float n, float f)
{
	makeIdent();
	a[0] = n;
	b[1] = n;
	c[2] = n + f;
	c[3] = -(f*n);
	d[2] = 1.0;
	d[3] = 0.0;
}


void LMatrix::pointMult(LPoint * p)
{
	float * v = p->getCoords(); 
	float ix = a[0]*v[0] + a[1]*v[1] + a[2]*v[2] + a[3]*v[3];
	float iy = b[0]*v[0] + b[1]*v[1] + b[2]*v[2] + b[3]*v[3];
	float iz = c[0]*v[0] + c[1]*v[1] + c[2]*v[2] + c[3]*v[3];
	float iw = d[0]*v[0] + d[1]*v[1] + d[2]*v[2] + d[3]*v[3];
	p->setPosition(ix, iy, iz, iw); //check p here
} //check p again


void LMatrix::vectorMult(LVector * vec)
{
	float * v = vec->getCoords();
	float ix = a[0]*v[0] + a[1]*v[1]  + a[2]*v[2];
	float iy = b[0]*v[0] + b[1]*v[1]  + b[2]*v[2];
	float iz = c[0]*v[0] + c[1]*v[1]  + c[2]*v[2];
	vec->setPosition(ix, iy, iz);
}


void LMatrix::makeIdent()
{
	float newA[] = {1.0, 0.0, 0.0, 0.0};
	float newB[] = {0.0, 1.0, 0.0, 0.0};
	float newC[] = {0.0, 0.0, 1.0, 0.0};
	float newD[] = {0.0, 0.0, 0.0, 1.0};

	for (int i = 0; i < 4; ++i)
	{
		a[i] = newA[i];
		b[i] = newB[i];
		c[i] = newC[i];
		d[i] = newD[i];
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


///////////////////////////////////////

class CtoFMat
{
public:
	CtoFMat(){}

	CtoFMat(LPoint p1,LPoint p2,LPoint p3)
	{

		float a,b,c,d,e,f;

		c = p1.coords[0];
		a = p2.coords[0] - c;
		b = p3.coords[0] - c;

		f = p1.coords[1];
		d = p2.coords[1] - f;
		e = p3.coords[1] - f;

		float invDet = 1.0/(a*e - b*d);

		A[0] = e*invDet;
		A[1] = -b*invDet;
		A[2] = (b*f - c*e)*invDet;

		B[0] = -d*invDet;
		B[1] = a*invDet;
		B[2] = (c*d - a*f)*invDet;
		
	}

	~CtoFMat(){}

	void bG(float x, float y, float BG[3])
	{
		BG[0] = x*A[0] + y*A[1] + A[2];
		BG[1] = x*B[0] + y*B[1] + B[2];
		BG[2] = BG[0] + BG[1];
	}

private:
	float A[3], B[3];	
};

















#endif