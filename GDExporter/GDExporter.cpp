#include "GDExporter.h"
#include <maya/MFileIO.h>

GDExporter* GDExporter::instancePtr = 0;

MFnMesh* GDExporter::GetSelectedMesh(void)
{
	MSelectionList currSelection;
	MGlobal::getActiveSelectionList(currSelection);

	unsigned int selectionCount = currSelection.length();
	
	MFnMesh* exportingMesh = 0;

	for(unsigned int selectionIndex = 0; selectionIndex < selectionCount; ++selectionIndex )
	{
		MDagPath currPath;
		currSelection.getDagPath(selectionIndex, currPath);

		if( currPath.apiType() != MFn::kTransform )
			continue;

		MFnTransform currTransform(currPath);
		unsigned int childCount = currTransform.childCount();
		for(unsigned int childIndex = 0; childIndex < childCount; ++childIndex)
		{
			MObject childObject = currTransform.child(childIndex);
			if( childObject.apiType() == MFn::kMesh )
			{
				MFnDagNode dagNode;
				dagNode.setObject(childObject);
				MDagPath childPath;
				dagNode.getPath(childPath);
				
				MFnMesh* pChildMesh = new MFnMesh(childPath);
				
				if( pChildMesh->isIntermediateObject() )
					continue;
				
				bool bExportMesh = true;
				if( pChildMesh->isInstanced(false) )
					if( childPath.instanceNumber() != 0 )
						bExportMesh = false;

				if( bExportMesh )
				{
					if( exportingMesh != 0 )
					{
						delete exportingMesh;
						delete pChildMesh;
						MGlobal::displayError(MString("GDExporter - More than one mesh object selected."));
						return 0;
					}

					exportingMesh = pChildMesh;
				}
			}
		}
	}

	if( exportingMesh == 0 )
	{
		MGlobal::displayError(MString("GDExporter - No mesh objects currently selected."));
		return 0;
	}

	return exportingMesh;
}

bool GDExporter::GetSelectedMeshTransformPath(MDagPath &transformPath)
{
	MSelectionList currSelection;
	MGlobal::getActiveSelectionList(currSelection);

	unsigned int selectionCount = currSelection.length();
	
	MFnMesh* exportingMesh = 0;

	for(unsigned int selectionIndex = 0; selectionIndex < selectionCount; ++selectionIndex )
	{
		MDagPath currPath;
		currSelection.getDagPath(selectionIndex, currPath);

		if( currPath.apiType() != MFn::kTransform )
			continue;

		MFnTransform currTransform(currPath);
		unsigned int childCount = currTransform.childCount();
		for(unsigned int childIndex = 0; childIndex < childCount; ++childIndex)
		{
			MObject childObject = currTransform.child(childIndex);
			if( childObject.apiType() == MFn::kMesh )
			{
				MFnMesh childMesh(childObject);
				if( childMesh.isIntermediateObject() )
					continue;

				currTransform.getPath(transformPath);
				return true;
			}
		}
	}

	return false;
}

const char* GDExporter::GetSceneName(std::string &sceneName)
{
	std::string sceneFileName = MFileIO::currentFile().asChar();
	std::string::size_type nameStart = sceneFileName.find_last_of('/') + 1;
	std::string::size_type nameEnd = sceneFileName.find_first_of('.');
	sceneName = sceneFileName.substr( nameStart, nameEnd - nameStart );
	
	return (sceneName.length() == 0 ? 0 : sceneName.c_str());
}

void GDExporter::GetUniqueRenderContexts( MFnTransform& transform, std::list<GDRenderContext>& uniqueContexts )
{
	for(unsigned int i = 0; i < transform.childCount(); ++i)
	{
		if( transform.child(i).apiType() == MFn::kMesh )
		{
			MFnMesh childMesh( transform.child(i) );

			if( childMesh.isIntermediateObject() )
				continue;

			MDagPath meshPath;
			childMesh.getPath(meshPath);
			MFnDependencyNode depNode(meshPath.node());
			MStatus fpResult;
			MPlug gdRenderContextPlug = childMesh.findPlug("GDRenderContext", &fpResult);

			if( fpResult != MS::kSuccess )
				continue;

			GDRenderContext newContext(gdRenderContextPlug);

			bool bFound = false;
			std::list<GDRenderContext>::iterator uniqueIter = uniqueContexts.begin();
			for(; uniqueIter != uniqueContexts.end(); ++uniqueIter )
			{
				if( (*uniqueIter) == newContext )
				{
					bFound = true;
					break;
				}
			}

			if( bFound == false )
				uniqueContexts.push_back( newContext );
			else
				gdRenderContextPlug.child(0).setValue( (*uniqueIter).name.c_str() );
		}
		else if( transform.child(i).apiType() == MFn::kTransform )
		{
			MFnTransform childTransform( transform.child(i) );
			GetUniqueRenderContexts( childTransform, uniqueContexts );
		}
	}
}

bool CompareStrings(const std::string &lhs, const std::string &rhs)
{
	if( lhs.length() == 0 && rhs.length() == 0 )
		return true;
	if( lhs.length() != 0 && rhs.length() == 0 )
		return false;
	if( lhs.length() == 0 && rhs.length() != 0 )
		return false;

	return ( strcmp( lhs.c_str(), rhs.c_str() ) == 0 );
}

void MFnCompoundAttrAddString( MFnCompoundAttribute &attr, const char* full, const char* brief, const char* def )
{
	MStatus status;
	MFnTypedAttribute stringAttribute;
	MObject strinAttrObj = stringAttribute.create( full, brief, MFnData::kString, &status );
	
	// Default strings don't get saved, so setting values for defaults in AddRenderContextAttribute
	//if( def != 0 )
	//{
	//	MFnStringData defaultString;
	//	MObject defaultStringObj = defaultString.create(def);
	//	stringAttribute.setDefault(defaultStringObj);
	//}

	attr.addChild( strinAttrObj );
}

void MFnCompoundAttrAddBool( MFnCompoundAttribute &attr, const char* full, const char* brief, bool def )
{
	MStatus status;
	MFnNumericAttribute boolAttr;
	MObject boolObj = boolAttr.create( full, brief, MFnNumericData::kBoolean, (def? 1.0 : 0.0), &status );
	attr.addChild( boolObj );
}

void MFnCompoundAttrAddEnum( MFnCompoundAttribute &attr, const char* full, const char* brief, const char** pEnumStrings, unsigned short count, unsigned short def )
{
	MStatus status;
	MFnEnumAttribute enumAttr;
	MObject enumObj = enumAttr.create( full, brief, def, &status );

	for(short index = 0; index < count; ++index)
		enumAttr.addField( pEnumStrings[index], index );

	attr.addChild( enumObj );
}