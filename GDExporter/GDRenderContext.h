#pragma once

#include <string>
#include <iostream>
#include <fstream>

class MPlug;
class MFnMesh;

struct GDRenderContext
{
	enum GDRenderStyle{ GDRS_DEFERRED = 0, GDRS_FORWARD, GDRS_TRANSPARENT, GDRS_MAX };
	static const char* RenderStyleString[GDRS_MAX];

	enum GDVertexFormat{ GDVF_VERTEX_POS = 0, GDVF_VERTEX_POSCOLOR, GDVF_VERTEX_POSTEX, GDVF_VERTEX_POSNORMTEX, GDVF_VERTEX_POSNORMTANTEX, GDVF_MAX };
	static const char* VertexFormatString[GDVF_MAX];

	std::string name;
	std::string material;
	std::string effect;
	std::string technique;
	GDRenderStyle renderStyle;
	GDVertexFormat vertexFormat;
	bool m_CastShadows;

	bool operator==(const GDRenderContext& rhs)const;
	
	GDRenderContext(void){};
	GDRenderContext(MPlug &renderContextPlug);

	void CopyToMesh(MFnMesh &mesh);

	void WriteXML(void);
};

