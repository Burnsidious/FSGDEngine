#include "InfluenceAttribute.h"

template<>
void CollectAttribute<GDVAT_JOINTS>(const MFnMesh& mfnMesh, GDVertexAttributeSet* pAttributeSets)
{
	MStatus status;
	MPlug plug = mfnMesh.findPlug("inMesh", &status);
	MItDependencyGraph dgIt( plug, MFn::kInvalid, MItDependencyGraph::kUpstream, MItDependencyGraph::kDepthFirst, MItDependencyGraph::kPlugLevel, &status );

	if( status != MStatus::kSuccess )
		return;
		
	pAttributeSets[GDVAT_JOINTS].m_Attributes.resize(mfnMesh.numVertices());
	pAttributeSets[GDVAT_WEIGHTS].m_Attributes.resize(mfnMesh.numVertices());

	MDagPath meshPath;
	mfnMesh.getPath(meshPath);	

	dgIt.disablePruningOnFilter();
	for(; !dgIt.isDone(); dgIt.next())
	{
		MObject skinObject = dgIt.thisNode();
		if( skinObject.apiType() != MFn::kSkinClusterFilter )
			continue;

		MFnSkinCluster skinCluster(skinObject);
			
		MItGeometry geomIt(meshPath);
		for(; !geomIt.isDone(); geomIt.next())
		{
			MObject component = geomIt.component();
			int componentIndex = geomIt.index();

			GDVertexAttribute& jointAttribute = pAttributeSets[GDVAT_JOINTS].m_Attributes[componentIndex];
			GDVertexAttribute& weightAttribute = pAttributeSets[GDVAT_WEIGHTS].m_Attributes[componentIndex];
			
			MFloatArray weights;
			unsigned int influenceCount;
				
			skinCluster.getWeights(meshPath, component, weights, influenceCount);
				
			unsigned int weightCount = 0;
			for(unsigned int i = 0; i < weights.length() && weightCount < 4; ++i)
			{
				if( weights[i] <= 0.001f )
					continue;

				jointAttribute.joints[weightCount] = i;
				weightAttribute.m_v[weightCount] = weights[i];

				++weightCount;
			}
		}
	}	
}

template<>
void WriteAttributeXML<GDVAT_JOINTS>(std::ofstream &fout, GDVertex &vert)
{
	char buffer[256];
	sprintf_s( buffer, "<JointIndex>%d</JointIndex><JointIndex>%d</JointIndex><JointIndex>%d</JointIndex><JointIndex>%d</JointIndex>\n", vert.attributes[GDVAT_JOINTS].joint0, vert.attributes[GDVAT_JOINTS].joint1, vert.attributes[GDVAT_JOINTS].joint2, vert.attributes[GDVAT_JOINTS].joint3 );
	fout << buffer;
}

template<>
void WriteAttributeXML<GDVAT_WEIGHTS>(std::ofstream &fout, GDVertex &vert)
{
	char buffer[256];
	sprintf_s( buffer, "<WeightValue>%f</WeightValue>\n<WeightValue>%f</WeightValue>\n<WeightValue>%f</WeightValue>\n<WeightValue>%f</WeightValue>\n", vert.attributes[GDVAT_WEIGHTS].weight0, vert.attributes[GDVAT_WEIGHTS].weight1, vert.attributes[GDVAT_WEIGHTS].weight2, vert.attributes[GDVAT_WEIGHTS].weight3 );
	fout << buffer;
}

template<>
void WriteAttributeBinary<GDVAT_JOINTS>(std::ofstream &fout, GDVertex &vert)
{
	fout.write( (const char*)&vert.attributes[GDVAT_JOINTS].joint0, sizeof(vert.attributes[GDVAT_JOINTS].joint0)*4 );
}

template<>
void WriteAttributeBinary<GDVAT_WEIGHTS>(std::ofstream &fout, GDVertex &vert)
{
	fout.write( (const char*)&vert.attributes[GDVAT_WEIGHTS].joint0, sizeof(vert.attributes[GDVAT_JOINTS].weight0)*4 );
}

template<>
unsigned int AttributeByteCount<GDVAT_JOINTS>(void)
{
	return 16;
}

template<>
unsigned int AttributeByteCount<GDVAT_WEIGHTS>(void)
{
	return 16;
}