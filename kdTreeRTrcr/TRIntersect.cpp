/*
 *  I took code from http://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
 *  and made a very minor alteration to it
 *
 *	triCheck.cpp
 *  Created by Schuyler Kylstra
 * 	11/12/14
 *	comp 770
 *	Instructor: Dinesh Manoucha
 *
 */

#ifndef tri_check
#define tri_check

float gI, gJ;

#define EPSILON 0.00000001

void SUB(Vector3& edge, const Vector3& v2, const Vector3& v1)
{
	edge.x = v2.x - v1.x;
	edge.y = v2.y - v1.y;
	edge.z = v2.z - v1.z;
}

void CROSS(Vector3& cp, const Vector3& v2, const Vector3& v1)
{
	cp.x = v1.y * v2.z - v1.z * v2.y;
	cp.y = v1.z * v2.x - v1.x * v2.z;
	cp.z = v1.x * v2.y - v1.y * v2.x;
}

float DOT(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


bool triangle_intersection( const Vector3&   V1,  // Triangle vertices
							const Vector3&   V2,
							const Vector3&   V3,
							const Vector3&    O,
							const Vector3&	  D,
							float& 			out,
							float 			BG[2])
{

	Vector3 e1, e2;  //Edge1, Edge2
	Vector3 P, Q, T;
	float det, inv_det, u, v;
	float t;

	//Find vectors for two edges sharing V1
	
	SUB(e1, V2, V1);
	SUB(e2, V3, V1);

	//Begin calculating determinant - also used to calculate u parameter

	CROSS(P, D, e2);

	//if determinant is near zero, ray lies in plane of triangle

	det = DOT(e1, P);

	//NOT CULLING

	if(det > -EPSILON && det < EPSILON) return false;
	inv_det = 1.f / det;

	//calculate distance from V1 to ray origin

	SUB(T, O, V1);

	//Calculate u parameter and test bound
	u = DOT(T, P) * inv_det;
	//The intersection lies outside of the triangle

	if(u < 0.f || u > 1.f) return false;

	//Prepare to test v parameter

	CROSS(Q, T, e1);

	//Calculate V parameter and test bound

	v = DOT(D, Q) * inv_det;

	//The intersection lies outside of the triangle

	if(u < 0.f || v < 0.f || u + v  > 1.f) return false;


	//u and v are the barycentric coordinates

	t = DOT(e2, Q) * inv_det;

	if(t > EPSILON) 
	{ //ray intersection
		out = t;
		BG[0] = u;
		BG[1] = v;
		return true;
	}

	// No hit, no win
	return false;
}



#endif