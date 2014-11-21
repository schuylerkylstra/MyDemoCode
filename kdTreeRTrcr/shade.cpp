/*
 *
 *	intersect.cpp
 *  Created by Schuyler Kylstra
 * 	11/12/14
 *	comp 770
 *	Instructor: Dinesh Manoucha
 *
 */



#ifndef shade_cpp
#define shade_cpp

#define EP .01

Vector3 interpolate(const Vector3& v0, const Vector3& v1, const Vector3& v2, const float BG[2])
{
	return v0 + ((v1 - v0)*BG[0]) + ((v2 - v0)*BG[1]);
}

void (*colorize) (const Vector3&, const Vector3&, const int, const float, const float[2], float[3]);


void noShadows(const Vector3& O, const Vector3& Dir, const int index, const float t, const float BG[2], float c[3])
{
	int k0 = gTriangles[index].indices[0];
	int k1 = gTriangles[index].indices[1];
	int k2 = gTriangles[index].indices[2];
	
	Vector3 p0, p1, p2;
	Vector3 n0, n1, n2;

	Vector3 pN;

	Vector3 lD = (O + Dir * t)*-1;
	lD.normalize();

	p0 = gPositions[k0];
	p1 = gPositions[k1];
	p2 = gPositions[k2];

	n0 = gNormals[k0];
	n1 = gNormals[k1];
	n2 = gNormals[k2];

	CROSS(pN, p2 - p0, p1 - p0);
	pN.normalize();
	c[0] = 1.0 * std::max(0.f, DOT(lD, pN));
	c[1] = c[0];
	c[2] = c[0];
}


void shadows(const Vector3& O, const Vector3& Dir, const int index, const float t, const float BG[2], float c[3])
{
	noShadows(O, Dir, index, t, BG, c); 


	Vector3	Origin = {0.f, 0.f, 0.f};
	Vector3 lD = (O + Dir * t);
	float mag = sqrt(lD.x * lD.x + lD.y * lD.y + lD.z * lD.z);
	lD.normalize();

	int 	ind = -1;
	float 	ar[2] = {0};

	float baseline = mag - EP;

	rIntersection(Origin, lD, gKDTree[0], ind, mag, ar);

	if (mag < baseline)
	{
		c[0] = 0.f;
		c[1] = 0.f;
		c[2] = 0.f;
	}

}

#endif