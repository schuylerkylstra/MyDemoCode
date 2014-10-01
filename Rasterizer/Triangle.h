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

class Triangle
{
	Triangle(){}

	Triangle(LPoint p0, LPoint p1, LPoint p2)
	{
		points[0] = p0;
		points[1] = p1;
		points[2] = p2;
		setNormal();
	}

	~Triangle(){}

	void setNormal();

	LPoint * getPoints();

	LVector * getNormal();

	LPoint points[3];
	LVector normal;
};









#endif