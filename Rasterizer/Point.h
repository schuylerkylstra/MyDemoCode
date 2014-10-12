/* 
*
*	Point.h
*   Created by Schuyler Kylstra
* 	10/6/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/



#ifndef VIS_LPOINT_H
#define VIS_LPOINT_H

#define POINT_W 1

#include "Vector.h"
#include "Surface.h"


class LPoint
{
public:
	LPoint()
	{
		coords[0] = 0;
		coords[1] = 0;
		coords[2] = 0;
		coords[3] = 1;
	}

	LPoint(float ix, float iy, float iz)
	{
		coords[0] = ix;
		coords[1] = iy;
		coords[2] = iz;
		coords[3] = 1;
	}

	LVector pointDiff(LPoint);

	LVector operator-(LPoint);

	LPoint operator + (LPoint);

	LPoint operator * (float);

	void scalarMult(float);

	float * getCoords();

	void setPosition(float, float, float);

	void setPosition(float, float, float, float);

	RGB_Color colorize(LPoint, LPoint, Surface, LVector*);

	float coords[4];
};

void LPoint::setPosition(float ix, float iy, float iz)
{
	coords[0] = ix;
	coords[1] = iy;
	coords[2] = iz;
}


void LPoint::scalarMult(float c)
{
	coords[0]*=c;
	coords[1]*=c;
	coords[2]*=c;
	coords[3]*=c;
}


LPoint LPoint::operator + (LPoint p)
{
	return LPoint(coords[0] + p.coords[0], coords[1] + p.coords[1], coords[2] + p.coords[2]);
}


LPoint LPoint::operator * (float c)
{
	return LPoint(coords[0]*c, coords[1]*c, coords[2]*c);
}


void LPoint::setPosition(float ix, float iy, float iz, float iw)
{
	coords[0] = ix;//print this
	coords[1] = iy;
	coords[2] = iz;
	coords[3] = iw;
}


RGB_Color LPoint::colorize(LPoint light, LPoint camera, Surface s, LVector * normal)
{
	LVector l = light - *this;
	LVector v = camera - *this;
	LVector h = l + v;
	h.normalize();
	l.normalize();
	RGB_Color color = s.getRGBColor(l.dotProduct(normal), h.dotProduct(normal));
	color.gammaCorrect();
	return color;
}


float * LPoint::getCoords()
{
	return coords;
}



LVector LPoint::pointDiff(LPoint p)
{
	float * pCoords = p.getCoords();
	return LVector(coords[0] - pCoords[0], coords[1] - pCoords[1], coords[2] - pCoords[2]);
}

LVector LPoint::operator - (LPoint p)
{
	float * pCoords = p.getCoords();
	return LVector(coords[0] - pCoords[0], coords[1] - pCoords[1], coords[2] - pCoords[2]);
}






////////////////////////////////////////////////////////////////////////////////////////






class LVertex
{
public:
	LVertex()
	{
		location 	= 	LPoint();
		shareNum 	= 	0;
	}

	LVertex(float ix, float iy, float iz, LVector * v)
	{
		location 	= 	LPoint(ix, iy, iz);
		shareNum 	= 	1;
		normal 		= 	*v;
		normal.normalize();
	}

	LVertex(LPoint * p)
	{
		location 	= 	*p;
		shareNum 	= 	0;
	}

	void setNormal(LVector * );

	void includeNormal(LVector * );

	LPoint getLoc();

	void colorize(LPoint, LPoint, Surface);

	RGB_Color getColor();

	LVector getNormal();


private:
	LPoint location;
	LVector normal; //This will become the average of the normals of all of the faces that contain this vertex
	int shareNum; //The number of surfaces that share the vertex
	RGB_Color color; //NOT GAMMA CORRECTED
};

LPoint LVertex::getLoc()
{
	return location;
}

LVector LVertex::getNormal()
{
	return normal;
}

void LVertex::setNormal(LVector * v)
{
	normal = *v;
	normal.normalize();
	shareNum = 1;
}

void LVertex::includeNormal(LVector * v)
{
	LVector uV = v->unitVect();
	normal = (normal*((float) shareNum/(shareNum + 1))) + (uV)*(1.0/(shareNum + 1)); //Weighted average of the normalized vectors
	normal.normalize();
	shareNum++;
}

void LVertex::colorize(LPoint light, LPoint camera, Surface s)
{
	LVector l = light - location;
	LVector v = camera - location;
	l.normalize();
	v.normalize();
	LVector h = (l + v).unitVect();
	color = s.getRGBColor(l.dotProduct(&normal), h.dotProduct(&normal));
}

RGB_Color LVertex::getColor()
{
	return color;
}

#endif