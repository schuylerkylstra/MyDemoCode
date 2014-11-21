/* 
*
*	triple.h
*   Created by Schuyler Kylstra
* 	11/12/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/



#ifndef META_triple_H
#define META_triple_H

struct triple
{
	float XYZ[3];
	float X;
	float Y;
	float Z;


	triple()
	{
		this->X = 0.0;
		this->Y = 0.0;
		this->Z = 0.0;
	}


	triple(float inX, float inY, float inZ)
	{
		this->X = inX;
		this->Y = inY;
		this->Z = inZ;
	}


	~triple()
	{

	}


	float inline getMagSqrd()
	{
		return X*X + Y*Y + Z*Z;
	}

	void print()
	{
		printf("%f, %f, %f", X, Y, Z);
	}


	float operator * (triple t)
	{
		return X*t.X + Y*t.Y + Z*t.Z;
	}

	void operator = (triple t)
	{
		this->X = t.X;
		this->Y = t.Y;
		this->Z = t.Z;
	}
};


#endif


