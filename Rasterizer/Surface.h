/* 
*
*	Surface.h
*   Created by Schuyler Kylstra
* 	10/6/14
*	comp 770
*	Instructor: Dinesh Manoucha
*
*/




#ifndef SURFACE
#define SURFACE

#import <math.h>

struct RGB_Color
{
	float RGB[3];

	RGB_Color()
	{
		
	}


	RGB_Color(float inR, float inG, float inB)
	{
		RGB[0] = inR;
		RGB[1] = inG;
		RGB[2] = inB;
	}



	void scalarMult(float c)
	{
		if(c>=0)
		{
			RGB[0]*=c;
			RGB[1]*=c;
			RGB[2]*=c;
		}
	}



	float * getRGBColor()
	{
		return RGB;
	}



	RGB_Color operator+(RGB_Color c)
	{
		return RGB_Color(RGB[0] + c.RGB[0], RGB[1] + c.RGB[1], RGB[2] + c.RGB[2]);
	}



	RGB_Color operator-(RGB_Color c)
	{
		return RGB_Color(RGB[0] - c.RGB[0], RGB[1] - c.RGB[1], RGB[2] - c.RGB[2]);
	}



	RGB_Color operator/(float divisor)
	{
		
		if(divisor>0)
		{
			divisor = 1.0/divisor;
			return RGB_Color(RGB[0]*divisor, RGB[1]*divisor, RGB[2]*divisor);
		}
		return *this;
	}



	RGB_Color operator*(float c)
	{
		if(c>=0)
		{
			return RGB_Color(RGB[0]*c, RGB[1]*c, RGB[2]*c);
		}
		return *this;
	}

	void gammaCorrect()
	{
		// c = 1/c;
		for (int i = 0; i < 3; ++i)
		{
			RGB[i] = pow(RGB[i], .454545);
		}
	}


	void cap()
	{
		for (int i = 0; i < 3; ++i)
		{
			if(RGB[i] > 1.0)
			{
				RGB[i] = 1.0;
			}
		}
	}

	void print()
	{
		printf("CHECK2 %f, %f, %f\n",RGB[0],RGB[1],RGB[2] );
	}
};




struct Surface
{

	RGB_Color ambientColor;
	RGB_Color diffuseColor;
	RGB_Color specularColor;
	int specularPower;


	Surface()
	{
		specularPower = 0;
		ambientColor = RGB_Color(1.0, 1.0, 1.0);
		diffuseColor = RGB_Color(1.0, 1.0, 1.0);
		specularColor = RGB_Color(1.0, 1.0, 1.0);
		
	}


	Surface(RGB_Color amColor, RGB_Color difColor, RGB_Color speColor, int spPo)
	{
		ambientColor = amColor;
		diffuseColor = difColor;
		specularColor = speColor;
		specularPower = spPo;
	}


	~Surface()
	{

	}

	RGB_Color getRGBColor(float nDotL, float nDotH)
	{
		RGB_Color c = ambientColor + diffuseColor*fmax(0.0, nDotL) + specularColor*pow(fmax(0.0, nDotH), specularPower);
		return c;
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



#endif