/* 
*
*	spaceVector.h
*   Created by Schuyler Kylstra
* 	11/12/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef META_spaceVector_H
#define META_spaceVector_H

#include "triple.h"


struct spaceVector : public triple
{
	float X;
	float Y;
	float Z;

	spaceVector()
	{
		this->X = 0.0;
		this->Y = 0.0;
		this->Z = 0.0;
	}


	spaceVector(triple p1, triple p2)
	{
		this->X = p2.X - p1.X;
		this->Y = p2.Y - p1.Y;
		this->Z = p2.Z - p1.Z;
	}


	spaceVector(triple p1)
	{
		this->X = p1.X;
		this->Y = p1.Y;
		this->Z = p1.Z;
	}


	spaceVector(float iX, float iY, float iZ)
	{
		this->X = iX;
		this->Y = iY;
		this->Z = iZ;
	}


	~spaceVector()
	{

	}


	float  inline dotProduct(spaceVector v)
	{
		return (v.X*X + v.Y*Y + v.Z*Z);
	}


	float inline dotProduct(triple t)
	{
		return (t.X*X + t.Y*Y + t.Z*Z);
	}


	spaceVector inline crossProduct(spaceVector v)
	{
		return spaceVector(Y*v.Z - Z*v.Y, Z*v.X - X*v.Z, X*v.Y - Y*v.X);
	}


	spaceVector inline vectorDifference(spaceVector v)
	{
		return spaceVector(X - v.X, Y - v.Y, Z - v.Z);
	}


	spaceVector inline vectorSum(spaceVector v)
	{
		return spaceVector(X + v.X, Y + v.Y, Z + v.Z);
	}


	void normalize()
	{

		float magnitude  = sqrt(X*X + Y*Y + Z*Z);
		this->X = X/magnitude;
		this->Y = Y/magnitude;
		this->Z = Z/magnitude;
	}

	spaceVector normalized()
	{
		float magnitude  = sqrt(X*X + Y*Y + Z*Z);
		float x = X/magnitude;
		float y = Y/magnitude;
		float z = Z/magnitude;
		return spaceVector(x, y, z);
	}


	void print()
	{
		printf("(%f, %f, %f)\n", this->X, this->Y, this->Z);
		// std::cout << X << ", " << Y << ", " << Z << "\n";
	}


	float inline getMag()
	{
		return sqrt(X*X + Y*Y + Z*Z);
	}


	void scalarMult(float c)
	{
		X = c*X;
		Y = c*Y;
		Z = c*Z;
	}

	spaceVector operator + (spaceVector v)
	{
		return spaceVector(X + v.X, Y + v.Y, Z + v.Z);
	}

	spaceVector operator - (spaceVector v)
	{
		return spaceVector(X - v.X, Y - v.Y, Z - v.Z);
	}

	spaceVector operator + (triple t)
	{
		return spaceVector(X + t.X, Y + t.Y, Z + t.Z); 
	}

	spaceVector operator - (triple t)
	{
		return spaceVector(X - t.X, Y - t.Y, Z - t.Z); 
	}


	spaceVector operator * (float c)
	{
		return spaceVector(X*c, Y*c, Z*c);
	}


	float operator * (triple t)
	{
		return X*t.X + Y*t.Y + Z*t.Z;
	}


	void operator/ (float c)
	{
		if(c == 0)
		{
			return;
		}

		scalarMult(1/c);
	}
};

#endif
