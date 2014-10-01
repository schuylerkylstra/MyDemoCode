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

class LPoint
{
public:
	LPoint()
	{
		coords[0] = 0;
		coords[1] = 0;
		coords[2] = 0;
	}

	LPoint(float ix, float iy, float iz)
	{
		coords[0] = ix;
		coords[1] = iy;
		coords[2] = iz;
	}

	LVector pointDiff(LPoint);

	float * getCoords();

	void setPosition(float, float, float);


	float coords[3];
};


void LPoint::setPosition(float ix, float iy, float iz)
{
	coords[0] = ix;
	coords[1] = iy;
	coords[2] = iz;
}


float * LPoint::getCoords()
{
	return coords;
}



LVector LPoint::pointDiff(LPoint p)
{
	float pCoords = p.getCoords();
	return LVector(coords[0] - pCoords[0], coords[1] - pCoords[1], coords[2] - pCoords[2]);
}

////////////////////////////////////////////////////////////////////////////////////////

class LVertex
{
	LVertex()
	{
		location = LPoint();
		shareNum 	= 	1;
	}

	LVertex(float ix, float iy, float iz, LVector * v)
	{
		location 	= 	LPoint(ix, iy, iz);
		shareNum 	= 	1;
		normal 		= 	*v;
		normal.normalize();
	}

	void setNormal(LVector * );

	void includeNormal(LVector * );

	LPoint location;
	LVector normal; //This will become the average of the normals of all of the faces that contain this vertex
	int shareNum; //The number of surfaces that share the vertex
};

void LVertex::setNormal(LVector * v)
{
	normal = *v;
	normal.normalize();
	shareNum = 1;
}

void LVertex::includeNormal(LVector * v)
{
	LVector uV = v->unitVect();
	normal = (normal*((float) shareNum/(shareNum + 1))) + (uV)*(1.0/(shareNum + 1)); //Weighted average of the  normalized vectors
	shareNum++;
}


#endif