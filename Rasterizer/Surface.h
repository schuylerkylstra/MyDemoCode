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



	void operator+(RGB_Color c)
	{
		for (int i = 0; i < 3; ++i)
		{
			RGB[i]+= c.RGB[i];
		}
	}



	void operator/(float div)
	{
		if(div>0)
		{
			RGB[0]/=div;
			RGB[1]/=div;
			RGB[2]/=div;
		}
	}



	void operator*(float c)
	{
		if(c>=0)
		{
			RGB[0]*=c;
			RGB[1]*=c;
			RGB[2]*=c;
		}

	}

	void gammaCorrect(float c)
	{
		// c = 1/c;
		for (int i = 0; i < 4; ++i)
		{
			RGB[i] = pow(RGB[i], c);
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
		printf("CHECK2 %f, %f, %f\n",RGBA[0],RGBA[1],RGBA[2] );
	}
};




struct Surface
{

	RGB_Color ambientColor;

	RGB_Color diffuseColor;

	RGB_Color specularColor;
	int specularPower;


	surface()
	{
		specularPower = 0;
		ambientColor = RGB_Color(1.0, 1.0, 1.0);
		diffuseColor = RGB_Color(1.0, 1.0, 1.0);
		specularColor = RGB_Color(1.0, 1.0, 1.0);
		
	}


	surface(RGB_Color amColor, RGB_Color difColor, RGB_Color speColor, int spPo)
	{
		ambientColor = amColor;
		diffuseColor = difColor;
		specularColor = speColor;
		specularPower = spPo;
	}


	~surface()
	{

	}


	//NEVER USE THIS
	float * getRGBColor()
	{
		RGB_Color c = ambientColor + diffuseColor + specularColor;
		return c.getRGBColor();
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