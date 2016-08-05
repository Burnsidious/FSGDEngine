#pragma once

#include <string>
#include <vector>
#include "GDVertex.h"
#include "GDTriangle.h"

struct GDMeshObject
{
	std::string meshFilePath;
	std::string meshName;
	std::vector< unsigned int > indices;

	bool attributePresent[GDVAT_MAX];
	std::string attributeNames[GDVAT_MAX];
	std::vector<GDVertex> vertices;

	bool triAttributePresent[GDTAT_MAX];
	std::string triAttributeNames[GDTAT_MAX];
	std::vector< GDTriangle > triangles;
	
	GDMeshObject(void){}
};