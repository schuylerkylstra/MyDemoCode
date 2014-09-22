/* 
*
*	structures.h
*   Created by Schuyler Kylstra
* 	9/9/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/


#ifndef META_structures_H
#define META_structures_H


#include <math.h>
#include <iostream>
#include <float.h>

struct tuple
{
	float XYZ[3];
	float X;
	float Y;
	float Z;


	tuple()
	{
		this->X = 0.0;
		this->Y = 0.0;
		this->Z = 0.0;
		XYZ[0] = 0.0;
		XYZ[1] = 0.0;
		XYZ[2] = 0.0;
	}


	tuple(float inX, float inY, float inZ)
	{
		this->X = inX;
		this->Y = inY;
		this->Z = inZ;
		XYZ[0] = inX;
		XYZ[1] = inY;
		XYZ[2] = inZ;
	}


	// tuple(int intX, int intY, int intZ)
	// {
	// 	X = (float) intX;
	// 	Y = (float) intY;
	// 	Z = (float) intZ;
	// }


	~tuple()
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


	float operator * (tuple t)
	{
		return X*t.X + Y*t.Y + Z*t.Z;
	}
};




struct color_RGBA : tuple
{

	float RGBA[4];

	color_RGBA()
	{
		
	}

	color_RGBA(tuple p)
	{
		RGBA[0] = p.X;
		RGBA[1] = p.Y;
		RGBA[2] = p.Z;
		RGBA[3] = 1.0;
	}

	color_RGBA(float inR, float inG, float inB)
	{
		RGBA[0] = inR;
		RGBA[1] = inG;
		RGBA[2] = inB;
		RGBA[3] = 1.0;
	}


	color_RGBA(float inR, float inG, float inB, float inA)
	{
		RGBA[0] = inR;
		RGBA[1] = inG;
		RGBA[2] = inB;
		RGBA[3] = inA;
	}


	color_RGBA scalarMult(float c)
	{
		if(c<0)
		{
			return *this;
		}
		return color_RGBA(RGBA[0]*c, RGBA[1]*c, RGBA[2]*c, RGBA[3]*c);
	}



	float * getRGBColor()
	{
		return RGBA;
	}

	void operator+(color_RGBA c)
	{
		for (int i = 0; i < 4; ++i)
		{
			RGBA[i]+= c.RGBA[i];
		}
	}

	void operator/(float div)
	{
		if(div<=0)
		{
			return;
		}
		for (int i = 0; i < 4; ++i)
		{
			RGBA[i] = RGBA[i]/div;
		}
	}



	color_RGBA operator*(float c)
	{
		if(c<0)
		{
			return *this;
		}
		return color_RGBA(RGBA[0]*c, RGBA[1]*c, RGBA[2]*c, RGBA[3]*c);
	}

	void gammaCorrect(float c)
	{
		// c = 1/c;
		for (int i = 0; i < 4; ++i)
		{
			RGBA[i] = pow(RGBA[i], c);
		}
	}


	void cap()
	{
		for (int i = 0; i < 4; ++i)
		{
			if(RGBA[i] > 1.0)
			{
				RGBA[i] = 1.0;
			}
		}
	}

	void print()
	{
		printf("CHECK2 %f, %f, %f\n",RGBA[0],RGBA[1],RGBA[2] );
	}
};




struct spaceVector : public tuple
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


	spaceVector(tuple p1, tuple p2)
	{
		this->X = p2.X - p1.X;
		this->Y = p2.Y - p1.Y;
		this->Z = p2.Z - p1.Z;
	}


	spaceVector(tuple p1)
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


	float inline dotProduct(tuple t)
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

	spaceVector operator + (tuple t)
	{
		return spaceVector(X + t.X, Y + t.Y, Z + t.Z); 
	}

	spaceVector operator - (tuple t)
	{
		return spaceVector(X - t.X, Y - t.Y, Z - t.Z); 
	}


	spaceVector operator * (float c)
	{
		return spaceVector(X*c, Y*c, Z*c);
	}


	float operator * (tuple t)
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




struct ray 
{
	tuple origin;
	spaceVector direction;
	float tMin;
	float tMax;


	ray()
	{
		tMin = 0.0;
		tMax = 100.f;
		origin = tuple();
		direction = spaceVector();
	}


	ray(tuple orig, spaceVector dir, float min, float max)
	{
		origin = orig;
		direction = dir;
		tMin = min;
		tMax = max;
	}

	ray(spaceVector orig, spaceVector dir, float min, float max)
	{
		ray(tuple(orig.X, orig.Y, orig.Z), dir, min, max);
	}


	~ray()
	{

	}


	void inline setDirection(spaceVector newDir)
	{
		direction = newDir;
		// direction.normalize();
	}


	ray changeDirection(spaceVector delta)
	{
		return ray(origin, direction.vectorSum(delta), tMin, tMax);
	}


	void inline normalize()
	{
		direction.normalize();
	}

	ray inline operator * (float c)
	{
		return ray(origin, direction*c, tMin, tMax);
	}

};



struct surface
{

	color_RGBA ambientColor;
	float ambientCoefficient;

	color_RGBA diffuseColor;
	float diffuseCoefficient;

	color_RGBA specularColor;
	float specularCoefficient;
	int specularPower;


	surface()
	{
		ambientCoefficient = .2;
		diffuseCoefficient = 1.0;
		specularCoefficient = 0;
		specularPower = 0;
		ambientColor = color_RGBA(1.0, 1.0, 1.0, 1.0)*ambientCoefficient;
		diffuseColor = color_RGBA(1.0, 1.0, 1.0, 1.0)*diffuseCoefficient;
		specularColor = color_RGBA(1.0, 1.0, 1.0, 1.0)*specularCoefficient;
		
	}


	surface(color_RGBA color, float amCo, float diCo, float spCo, int spPo)
	{
		ambientColor = color*amCo;
		diffuseColor = color*diCo;
		specularColor = color_RGBA(1.0, 1.0, 1.0, 1.0)*spCo;
		ambientCoefficient = amCo;
		diffuseCoefficient = diCo;
		specularCoefficient = spCo;
		specularPower = spPo;
	}

	surface(tuple color, float aC, float dC, float sC, int sP)
	{
		surface(color_RGBA(color), aC, dC, sC, sP);
	}


	~surface()
	{

	}

	float * getRGBColor()
	{
		return getAmColor();
	}

	float * getAmColor()
	{
		return ambientColor.getRGBColor();
	}

	float * getDiffColor()
	{
		return diffuseColor.getRGBColor();
	}


	float * getSpecColor()
	{
		return specularColor.getRGBColor();
	}

	int getSpecPower()
	{
		return specularPower;
	}
};

struct seenPair
{
	int index;
	float T;

	seenPair()
	{
		index = 4;
		T = FLT_MAX;
	}

	seenPair(int i, float f)
	{
		index = i;
		T = f;
	}

	~seenPair()
	{

	}
};


#endif