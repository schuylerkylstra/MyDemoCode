/* 
*
*	Triangle.h
*   Created by Schuyler Kylstra
* 	10/6/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef STR_TRI
#define STR_TRI

#import "Point.h"
#import "Vector.h"
#import "Surface.h"
#import <math.h>

class Triangle
{
public:
	Triangle(){}

	Triangle(LPoint * p0, LPoint * p1, LPoint * p2)
	{
		points[0] = p0;
		points[1] = p1;
		points[2] = p2;
		setNormalP();
		findCenter();
		trueIfPoints = true;
	}

	Triangle(LVertex * v0, LVertex * v1, LVertex * v2)
	{
		verts[0] = v0;
		verts[1] = v1;
		verts[2] = v2;
		setNormalV();
		v0->includeNormal(&normal);
		v1->includeNormal(&normal);
		v2->includeNormal(&normal);
		trueIfPoints = false;
	}

	~Triangle(){}

	LVector * getNormal();

	RGB_Color getColor();

	bool visible(LPoint);

	void colorize(LPoint, LPoint, Surface);

	LVertex ** getVerts();

	void colorizeVerts(LPoint, LPoint, Surface);

	
private:

	void findCenter();

	void setNormalP();

	void setNormalV();

	bool trueIfPoints;
	LVertex * verts[3];
	LPoint * points[3];
	LVector normal;
	RGB_Color color;
	LPoint center;
};

LVertex ** Triangle::getVerts()
{
	return verts;
}

void Triangle::colorizeVerts(LPoint light, LPoint camera, Surface s)
{
	verts[0]->colorize(light, camera, s);
	verts[1]->colorize(light, camera, s);
	verts[2]->colorize(light, camera, s);
}

RGB_Color Triangle::getColor()
{
	return color;
}

bool Triangle::visible(LPoint camera)
{
	if(trueIfPoints)
	{
		LVector v = camera - center;
		return (fmax(0, v.dotProduct(&normal))>0);
	}
	return true;
}

void Triangle::colorize(LPoint light, LPoint camera, Surface s)
{
	LVector l = light - center;
	LVector v = camera - center;
	l.normalize();
	v.normalize();
	LVector h = (l + v).unitVect();
	color = s.getRGBColor(normal.dotProduct(&l), normal.dotProduct(&h));
	color.gammaCorrect();
}


void Triangle::findCenter()
{
	center = (*points[0] + *points[1] + *points[2])*(1.0/3.); 
}

void Triangle::setNormalP()
{
	LVector v1 = *points[1] - *points[0];
	LVector v2 = *points[2] - *points[1];
	normal = v1.crossProduct(&v2);
	normal.normalize();
}

void Triangle::setNormalV()
{
	LVector v1 = verts[1]->getLoc() - verts[0]->getLoc();
	LVector v2 = verts[2]->getLoc() - verts[1]->getLoc();
	normal = v1.crossProduct(&v2);
	normal.normalize();
}


LVector * Triangle::getNormal()
{
	return &normal;
}


#endif