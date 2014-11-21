/*
	How to use this code:

	Call load_mesh("sibenik.obj") after creating the window using GLUT. This will
	read the mesh data from sibenik.obj and populate the arrays gPositions,
	gNormals, and gTriangles.

	When rendering, we use a similar convention to the sphere from PA2. In other
	words, for triangle i, define:

		k0 = gTriangles[i].indices[0];
		k1 = gTriangles[i].indices[1];
		k2 = gTriangles[i].indices[2];

	Then the vertices of the triangle are at gPositions[k0], gPositions[k1], and
	gPositions[k2], in that order.
*/

#ifndef LM
#define LM

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <float.h>


struct Vector3
{
	float			x, y, z;

	Vector3 operator - (const Vector3 v) const
	{
		Vector3 u = {x - v.x, y - v.y, z - v.z};
		return u;
	}

	Vector3 operator + (const Vector3 v) const
	{
		Vector3 u = {x + v.x, y + v.y, z + v.z};
		return u;
	}

	Vector3 operator * (float f) const
	{
		Vector3 u = {x * f, y * f, z * f};
		return u;
	}

	void normalize()
	{
		float inv_mag = 1.0 / sqrt(x*x + y*y + z*z);
		x = x*inv_mag;
		y = y*inv_mag;
		z = z*inv_mag;
	}


};

struct Triangle
{
	unsigned int 	indices[3];
};

struct KDNode
{
	bool				inner;
	float				minX, minY, minZ;
	float				maxX, maxY, maxZ;
	int 				indices[2];
	std::vector<int>	lTriBuffer;
};


std::vector<Vector3>	gPositions;
std::vector<Vector3>	gNormals;
std::vector<Triangle>	gTriangles;
std::vector<KDNode>		gKDTree;

void tokenize(char* string, std::vector<std::string>& tokens, const char* delimiter)
{
	char* token = strtok(string, delimiter);
	while (token != NULL)
	{
		tokens.push_back(std::string(token));
		token = strtok(NULL, delimiter);
	}
}

int face_index(const char* string)
{
	int length = strlen(string);
	char* copy = new char[length + 1];
	memset(copy, 0, length+1);
	strcpy(copy, string);

	std::vector<std::string> tokens;
	tokenize(copy, tokens, "/");
	delete[] copy;
	return atoi(tokens.front().c_str());
}

void load_mesh(std::string fileName)
{
	std::ifstream fin(fileName.c_str());
	if (!fin.is_open())
	{
		printf("ERROR: Unable to load mesh from %s!\n", fileName.c_str());
		exit(0);
	}

	float xmin = FLT_MAX;
	float xmax = -FLT_MAX;
	float ymin = FLT_MAX;
	float ymax = -FLT_MAX;
	float zmin = FLT_MAX;
	float zmax = -FLT_MAX;

	while (true)
	{
		char line[1024] = {0};
		fin.getline(line, 1024);

		if (fin.eof())
			break;

		if (strlen(line) <= 1)
			continue;

		std::vector<std::string> tokens;
		tokenize(line, tokens, " ");

		if (tokens[0] == "v")
		{
			float x = atof(tokens[1].c_str());
			float y = atof(tokens[2].c_str());
			float z = atof(tokens[3].c_str());

			xmin = std::min(x, xmin);
			xmax = std::max(x, xmax);
			ymin = std::min(y, ymin);
			ymax = std::max(y, ymax);
			zmin = std::min(z, zmin);
			zmax = std::max(z, zmax);

			Vector3 position = {x, y, z};
			gPositions.push_back(position);
		}
		else if (tokens[0] == "vn")
		{
			float x = atof(tokens[1].c_str());
			float y = atof(tokens[2].c_str());
			float z = atof(tokens[3].c_str());
			Vector3 normal = {x, y, z};
			normal.normalize();
			gNormals.push_back(normal);
		}
		else if (tokens[0] == "f")
		{
			unsigned int a = face_index(tokens[1].c_str());
			unsigned int b = face_index(tokens[2].c_str());
			unsigned int c = face_index(tokens[3].c_str());
			Triangle triangle;
			triangle.indices[0] = a - 1;
			triangle.indices[1] = b - 1;
			triangle.indices[2] = c - 1;
			gTriangles.push_back(triangle);
		}
	}

	fin.close();

	printf("Loaded mesh from %s. (%lu vertices, %lu normals, %lu triangles)\n", fileName.c_str(), gPositions.size(), gNormals.size(), gTriangles.size());
	printf("Mesh bounding box is: (%0.4f, %0.4f, %0.4f) to (%0.4f, %0.4f, %0.4f)\n", xmin, ymin, zmin, xmax, ymax, zmax);
}


void load_tree(std::string fileName)
{
	std::ifstream fin(fileName.c_str());
	if (!fin.is_open())
	{
		printf("ERROR: Unable to load mesh from %s!\n", fileName.c_str());
		exit(0);
	}

	while (true)
	{
		char line[8192] = {0};
		fin.getline(line, 8192);

		if (fin.eof())
			break;

		if (strlen(line) <= 1)
			continue;

		std::vector<std::string> tokens;
		tokenize(line, tokens, " ");

		if (tokens[0] == "inner{")
		{
			float minX 	= atof(tokens[1].c_str());
			float minY 	= atof(tokens[2].c_str());
			float minZ 	= atof(tokens[3].c_str());
			float maxX 	= atof(tokens[4].c_str());
			float maxY 	= atof(tokens[5].c_str());
			float maxZ 	= atof(tokens[6].c_str());
			int a 		= atoi(tokens[8].c_str());
			int b 		= atoi(tokens[9].c_str());

			KDNode inner;
			inner.inner= true;
			inner.minX = minX;
			inner.minY = minY;
			inner.minZ = minZ;
			inner.maxX = maxX;
			inner.maxY = maxY;
			inner.maxZ = maxZ;
			inner.indices[0] = a;
			inner.indices[1] = b;

			gKDTree.push_back(inner);
		}

		else if (tokens[0] == "leaf{")
		{
			float minX = atof(tokens[1].c_str());
			float minY = atof(tokens[2].c_str());
			float minZ = atof(tokens[3].c_str());
			float maxX = atof(tokens[4].c_str());
			float maxY = atof(tokens[5].c_str());
			float maxZ = atof(tokens[6].c_str());
			std::vector<int> indicies;

			for (int i = 8; i < tokens.size() - 1; ++i)
			{
				indicies.push_back(atoi(tokens[i].c_str()));
			}

			KDNode leaf;
			leaf.inner= false;
			leaf.minX = minX;
			leaf.minY = minY;
			leaf.minZ = minZ;
			leaf.maxX = maxX;
			leaf.maxY = maxY;
			leaf.maxZ = maxZ;
			leaf.lTriBuffer = indicies;
			gKDTree.push_back(leaf);
		}
	}

	fin.close();


	printf("Loaded tree from %s. %lu nodes.\n", fileName.c_str(), gKDTree.size());
}


#endif