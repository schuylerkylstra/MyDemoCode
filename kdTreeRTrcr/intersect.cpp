/*
 *
 *	intersect.cpp
 *  Created by Schuyler Kylstra
 * 	11/12/14
 *	comp 770
 *	Instructor: Dinesh Manoucha
 *
 */



#ifndef intersection
#define intersection

#include <stdio.h>
#include "structures.h"
#include "loadMesh.cpp"
#include "TRIntersect.cpp"




float inline intersectTime(float fin, float ini, float delta) // from parametric form of vectors
{
	if (delta > -EPSILON && delta < EPSILON)
	{
		return FLT_MAX;
	}
	return (fin - ini)/delta;
}




float impactTime(const Vector3& O, const Vector3& D, const KDNode node)
{
	float txMin , 	txMax;
	float tyMin , 	tyMax;
	float tzMin , 	tzMax;
	float maxMin,	minMax;

	float tXVals[2];
	float tYVals[2];
	float tZVals[2];

	tXVals[0]  = intersectTime(node.minX, O.x, D.x);
	tXVals[1]  = intersectTime(node.maxX, O.x, D.x);

	tYVals[0]  = intersectTime(node.minY, O.y, D.y);
	tYVals[1]  = intersectTime(node.maxY, O.y, D.y);

	tZVals[0]  = intersectTime(node.minZ, O.z, D.z);
	tZVals[1]  = intersectTime(node.maxZ, O.z, D.z);


	txMin = std::min(tXVals[0], tXVals[1]);
	txMax = std::max(tXVals[0], tXVals[1]);

	tyMin = std::min(tYVals[0], tYVals[1]);
	tyMax = std::max(tYVals[0], tYVals[1]);

	tzMin = std::min(tZVals[0], tZVals[1]);
	tzMax = std::max(tZVals[0], tZVals[1]);
	

	maxMin = std::max(tzMin, std::max(txMin, tyMin));
	minMax = std::min(txMax, std::min(tyMax, tzMax));


	if(maxMin <= minMax and minMax > 0.f)
	{
		return std::max(0.f, maxMin);
	}
	return FLT_MAX;
}


	

bool findTriHits(const Vector3& O, const Vector3& D, const std::vector<int>& v, int& index, float& t, float BG[2])
{

	bool 	hit 	= false;
	float 	minT 	= t;
	float 	tempT;
	int 	k0, k1, k2;
	int 	in;
	int 	tIndex = -1;
	Vector3 v0, v1, v2;

	for (int i = 0; i < v.size(); ++i)
	{
		in = v[i];
		
		

		k0 = gTriangles[in].indices[0];
		k1 = gTriangles[in].indices[1];
		k2 = gTriangles[in].indices[2];

		v0 = gPositions[k0];
		v1 = gPositions[k1];
		v2 = gPositions[k2];


		if (triangle_intersection(v0, v1, v2, O, D, tempT, BG) && (tempT < minT))
		{
			tIndex = in;
			minT = tempT;
			hit = true;
		}
	}

	t = minT;
	index = tIndex;

	return hit;
}



int lookAhead(const Vector3& O, const Vector3& D, const int indices[2])
{
	KDNode n0, n1;
	n0 = gKDTree[indices[0]];
	n1 = gKDTree[indices[1]];
	float iT0, iT1;

	

	iT0 = impactTime(O, D, n0);
	iT1 = impactTime(O, D, n1);

	int index;

	index = (iT0 <= iT1) ? 0 : 1;

	return index;
}



bool rIntersection(const Vector3& O, const Vector3& D, const KDNode node, int& index, float& t, float BG[2]) 
{

	float impact;
	bool hit		= false;

	impact = impactTime(O, D, node);

	if(impact < t)
	{
		
		if (node.inner) 
		{

			float t0(FLT_MAX);
			float t1(FLT_MAX);
			bool hit0(false), hit1(false);
			int index0(-1), index1(-1);
			float BG0[2];
			float BG1[2];

			hit0 = rIntersection(O, D, gKDTree[node.indices[0]], index0, t0, BG0);

			hit1 = rIntersection(O, D, gKDTree[node.indices[1]], index1, t1, BG1);

			if(t0 < t1 and hit0)
			{
				t 		= t0;
				index 	= index0;
				BG 		= BG0;
				return hit0;
			}

			else if (t1 < t0 and hit1)
			{
				t 		= t1;
				index 	= index1;
				BG 		= BG1;
				return hit1;
			}

			else if (t1 == t0 and index0 == index1)
			{
				t 		= t0;
				index 	= index0;
				BG 		= BG0;
				return hit0;	
			}
		
			return false;


		}

		else
		{
			hit = findTriHits(O, D, node.lTriBuffer, index, t, BG);
		}
	}
	return hit;
}







#endif