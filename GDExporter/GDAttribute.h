#pragma once

#include <iostream>
#include <fstream>
#include <vector>

enum GDVertexAttributeType{ GDVAT_INVALID = -1, GDVAT_POINTS, GDVAT_NORMALS, GDVAT_TANGENTS, GDVAT_BITANGENTS, GDVAT_TEXCOORDS_0, GDVAT_TEXCOORDS_1, GDVAT_TEXCOORDS_2, GDVAT_JOINTS, GDVAT_WEIGHTS, GDVAT_MAX };
enum GDTriangleAttributeType{ GDTAT_INVALID = -1, GDTAT_TRINORMS, GDTAT_TRINEIGHBORS, GDTAT_MAX };
enum GDMeshAttributeType{ GDMAT_INVALID = -1, GDMAT_SPHERE, GDMAT_AABB, GDMAT_BVH, GDMAT_MAX };

extern const char* GDVertexAttribTypeNames[GDVAT_MAX];
extern const char* GDTriangleAttribTypeNames[GDTAT_MAX];
extern const char* GDMeshAttributeTypeNames[GDMAT_MAX];

void WriteVector4fXML(std::ofstream& fout, const float *pVector);
void WriteTransformXML(std::ofstream& fout, const float transform[4][4]);

//void WriteAabbXML(std::ofstream& fout, const EDCollision::Aabb* aabbPtr);