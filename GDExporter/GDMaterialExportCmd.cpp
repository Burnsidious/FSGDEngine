#include "GDMaterialExportCmd.h"
#include <maya/MSyntax.h>
#include <maya/MSelectionList.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MFnTransform.h>
#include <maya/MDagPath.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MColor.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnData.h>
#include <maya/MFileIO.h>

#include "GDExporter.h"

using namespace std;

const char* m_MayaMatNames[6] = { "ambientColor", "color", "specularColor", "incandescence", "rc", "bumpValue" };
const char* m_MatNames[6] = { "Ambient", "Diffuse", "Specular", "Emmisive", "Reflected", "Normal" };

void *GDMaterialExportCmd::creator(void)
{
	return new GDMaterialExportCmd;
}

MSyntax GDMaterialExportCmd::newSyntax(void)
{
	MSyntax syntax;

	return syntax;
}

MStatus GDMaterialExportCmd::doIt(const MArgList &args)
{
	Reset();

	const char* exportPath = GDExporter::GetInstance()->EstablishExportPath();

	if( exportPath == 0 )
	{
		MGlobal::displayError(MString("GDExport - No export path established."));
		return MS::kFailure;
	}

	MDagPath meshTransformPath;
	if( GDExporter::GetInstance()->GetSelectedMeshTransformPath( meshTransformPath ) )
	{
		MFnTransform selectedTransform(meshTransformPath);
		GetUniqueMaterials(selectedTransform);
	}
	else
		return MS::kFailure;
	
	std::list<GDMaterial>::iterator uniqueIter = m_UniqueMaterials.begin();
	for(; uniqueIter != m_UniqueMaterials.end(); ++uniqueIter )
	{
		std::string matFilePath = exportPath;
		matFilePath += (*uniqueIter).m_MaterialFilePath;

		if( !GDExporter::GetInstance()->ValidateFileCreation( matFilePath.c_str() ) )
		{
			MGlobal::displayInfo( MString((*uniqueIter).m_MaterialName.c_str()) + " export canceled." );
			continue;
		}
		
		std::ofstream fout( matFilePath.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
		(*uniqueIter).WriteXML( fout );
		fout.close();
	}
	
	return MS::kSuccess;
}

void GDMaterialExportCmd::GetUniqueMaterials(MFnTransform &transform)
{
	const char* exportPath = GDExporter::GetInstance()->EstablishExportPath();
	
	if( exportPath == 0 )
		return;

	for(unsigned int i = 0; i < transform.childCount(); ++i)
	{
		if( transform.child(i).apiType() == MFn::kMesh )
		{
			MFnMesh childMesh( transform.child(i) );

			if( childMesh.isIntermediateObject() )
				continue;

			GDMaterial meshMaterial;
			GetMaterialFromMesh( childMesh, meshMaterial );

			MDagPath meshPath;
			childMesh.getPath(meshPath);
			MFnDependencyNode depNode(meshPath.node());
			MStatus fpResult;
			MPlug gdMaterialPlug = childMesh.findPlug("GDMaterial", &fpResult);
	
			if( fpResult != MS::kSuccess )
			{
				MStatus result;
				MFnTypedAttribute typedAttr;
				MObject newAttribute= typedAttr.create( "GDMaterial", "GDMat", MFnData::kString, &result);
				typedAttr.setHidden(false);
				childMesh.addAttribute( newAttribute );

				gdMaterialPlug = childMesh.findPlug("GDMaterial", &fpResult);
			}

			std::string materialFileName = "/GDMaterial";

			materialFileName += "/";

			std::string folderPath = exportPath;
			folderPath += materialFileName;
			_mkdir( folderPath.c_str() );
			
			std::string sceneName;
			GDExporter::GetInstance()->GetSceneName(sceneName);

			materialFileName += sceneName.c_str();
			
			folderPath += sceneName.c_str();

			_mkdir( folderPath.c_str() );

			materialFileName += "/mat_";
			
			bool bFound = false;
			std::list<GDMaterial>::iterator matIter = m_UniqueMaterials.begin();
			for(; matIter != m_UniqueMaterials.end(); ++matIter)
			{
				if( (*matIter) == meshMaterial )
				{
					bFound = true;
					materialFileName += (*matIter).m_MaterialName.c_str();
					materialFileName += ".xml";
					break;
				}
			}

			if( bFound == false )
			{
				materialFileName += meshMaterial.m_MaterialName.c_str();
				materialFileName += ".xml";
				meshMaterial.m_MaterialFilePath = materialFileName;
				m_UniqueMaterials.push_back( meshMaterial );
			}

			if( fpResult == MS::kSuccess )
				gdMaterialPlug.setValue( materialFileName.c_str() );
		}
		else if( transform.child(i).apiType() == MFn::kTransform )
		{
			MFnTransform childTransform( transform.child(i) );
			GetUniqueMaterials(childTransform);
		}
	}
}


void GDMaterialExportCmd::GetMaterialFromMesh(MFnMesh &fn_mesh, GDMaterial &mat)
{
	MObjectArray sets;
	MObjectArray comps;

	MDagPath path;
	fn_mesh.getPath(path);

	fn_mesh.getConnectedSetsAndMembers(path.instanceNumber(), sets, comps, true);

	int len = sets.length();
	for (unsigned int i = 0; i < sets.length(); i++)
	{
		MObject set = sets[i];
		MObject comp = comps[i];

		MFnDependencyNode fnd(sets[i]);
		MStatus status;
		
		// Is there a shader connected to it at all?
		MPlug mat_plug = fnd.findPlug("surfaceShader", &status);
		if (!status)
			continue;


		// Get the connections (materials) to this plug.
		MPlugArray connected_plugs;
		mat_plug.connectedTo(connected_plugs, true, false, &status);

		if (!status)
			continue;

		for (unsigned int j = 0; j < connected_plugs.length(); j++)
		{
			// Make a dependency node that will be the material.
			MFnDependencyNode fn_mat(connected_plugs[j].node());

			mat.m_MaterialName = fn_mat.name().asChar();
			
			for(unsigned int k = 0; k < GDM_NORMAL; ++k)
				GetTextureFromMaterial(fn_mat, k, mat);
			
			GetNormalTextureFromMaterial(fn_mat, mat);		
		}
	}
}

int GDMaterialExportCmd::ExportTexture(MString &fullPathTextureName)
{
	string exportPath = GDExporter::GetInstance()->GetExportPath();
	string sysCommand;
	MStringArray splitStrings;

	// Before we can copy the textures used to the textures folder, we need to create the directory
	string outputFolder = exportPath + "/Textures/";
	_mkdir(outputFolder.c_str());

	// Next we will build a string that will contain the system command needed to copy the texture 
	// to its new home
	sysCommand = "copy ";
	sysCommand += "\"";
	sysCommand += fullPathTextureName.asChar();
	sysCommand += "\"";
	sysCommand += ' ';
	sysCommand += "\"";
	sysCommand += outputFolder;
	sysCommand += "\"";

	// System commands need back slashes not forward slashes
	string::size_type loc;
	loc = sysCommand.find("/");

	while (loc != string::npos)
	{
		sysCommand.replace(loc, 1, "\\");
		loc = sysCommand.find("/");
	} 

	// Copy the textures to their new homes, and check for success
	int ret = system(sysCommand.c_str());
	if(ret)
		return ret;

	// Build the string path to the new texture location
	MString newTextureLoc(outputFolder.c_str());
	fullPathTextureName.split('/', splitStrings);
	unsigned int numSplits = splitStrings.length();
	// Add the texture name to the texture location
	newTextureLoc += splitStrings[numSplits-1];

	// Get the textures extension
	MStringArray extSplitStrings;
	newTextureLoc.split('.', extSplitStrings);
	numSplits = extSplitStrings.length();
	const char *extension = extSplitStrings[extSplitStrings.length()-1].asChar();
	
	// Test if the texture is already teh DDS format, if not convert
	if(strncmp(extension, "dds", 3))
	{
		// Build a system command string to execute the convert method supplied by ImageMagick
		// Example convert string : convert "somePath/someTexture.jpg" "somePath/someTexture.dds"
		sysCommand = "convert \"";
		sysCommand += newTextureLoc.asChar();
		sysCommand += "\" \"";

		for(unsigned int i = 0; i < numSplits-1; ++i)
		{
			sysCommand += extSplitStrings[i].asChar();
			sysCommand += '.';
		}
		sysCommand += "dds\"";

			// System commands need back slashes not forward slashes
		string::size_type loc;
		loc = sysCommand.find("/");

		while (loc != string::npos)
		{
			sysCommand.replace(loc, 1, "\\");
			loc = sysCommand.find("/");
		} 

		// Execute the convert command
		ret = system(sysCommand.c_str());
		if(ret)
			return ret;

		// Since we now have the dds, we no longer need the original in the textures folder
		// so we will build another system command to delete it
		sysCommand = "del ";
		sysCommand += "\"";
		MStringArray slashSplitStrings;

		newTextureLoc.split('/', slashSplitStrings);
		numSplits = slashSplitStrings.length();
		for(unsigned int i = 0; i < numSplits-1; ++i)
		{
			sysCommand += slashSplitStrings[i].asChar();
			sysCommand += "\\";
		}
		sysCommand += slashSplitStrings[numSplits-1].asChar();
		sysCommand += "\"";

		system(sysCommand.c_str());

	}
	return ret;
	//copy /y "C:\Users\Burnside\Documents\Projects\fsgdengine\x64\Debug\GDExporter.mll" "C:\Program Files\Autodesk\Maya2012\bin\plug-ins\GDExporter.mll"
}

void GDMaterialExportCmd::GetTextureFromMaterial(MFnDependencyNode &fn_mat, unsigned int index, GDMaterial &mat)
{
	MStatus status;
	MString texture = "";
	MString texture_type = MString(m_MayaMatNames[index]);

	MPlug color_plug;

	color_plug = fn_mat.findPlug( texture_type + "R", &status );
	if( status == MS::kSuccess )
		color_plug.getValue(mat.m_MaterialColors[index][0]);

	color_plug = fn_mat.findPlug( texture_type + "G", &status );
	if( status == MS::kSuccess )
		color_plug.getValue(mat.m_MaterialColors[index][1]);

	color_plug = fn_mat.findPlug( texture_type + "B", &status );
	if( status == MS::kSuccess )
		color_plug.getValue(mat.m_MaterialColors[index][2]);

	color_plug = fn_mat.findPlug( texture_type + "A", &status );
	if( status == MS::kSuccess )
		color_plug.getValue(mat.m_MaterialColors[index][3]);

	color_plug = fn_mat.findPlug(texture_type, &status);

	if (status)
	{
		MPlugArray connections;
		color_plug.connectedTo(connections, true, false);
		
		if (connections.length())
		{
			MObject src = connections[0].node();
			if (src.hasFn(MFn::kFileTexture))
			{
				MFnDependencyNode fn_file(src);
				MPlug file_plug = fn_file.findPlug("fileTextureName", &status);
				if (status == MS::kSuccess)
				{
					file_plug.getValue(texture);

					ExportTexture(texture);
				}
			}
		}
	}

	MStringArray splitStrings;
	texture.split('/', splitStrings);
	
	string texnamewithdds = splitStrings[ splitStrings.length() - 1].asChar();
	ReplaceExtensionWithDDS(texnamewithdds);
	mat.m_TextureFileNames[index] = texnamewithdds;

}

void GDMaterialExportCmd::GetNormalTextureFromMaterial(MFnDependencyNode &fn_mat, GDMaterial &mat)
{
	MStatus status;
	MString normal = "";
	MPlug normal_camera = fn_mat.findPlug("normalCamera", &status);
	if (status)
	{
		MPlugArray connections;
		normal_camera.connectedTo(connections, true, false);

		if (connections.length())
		{
			MPlug bump_value = MFnDependencyNode(connections[0].node()).findPlug("bumpValue", &status);
			if (status)
			{
				MPlugArray bv_connections;
				bump_value.connectedTo(bv_connections, true, false);
				if (connections.length())
				{
					MObject src = bv_connections[0].node();
					if (src.hasFn(MFn::kFileTexture))
					{
						MFnDependencyNode fn_file(src);
						MPlug file_plug = fn_file.findPlug("fileTextureName", &status);
						if (status == MS::kSuccess)
						{
							file_plug.getValue(normal);
							ExportTexture(normal);
						}
					}
				}
			}
		}
	}

	MStringArray splitStrings;
	normal.split('/', splitStrings);
	string texnamewithdds = splitStrings[ splitStrings.length() - 1].asChar();
	ReplaceExtensionWithDDS(texnamewithdds);
	mat.m_TextureFileNames[GDM_NORMAL] = texnamewithdds;
}

void GDMaterial::WriteXML(std::ofstream &fout)
{
	fout << "<?xml version=\"1.0\"?>\n";
	fout << "<GDMaterial name=\"" << m_MaterialName.c_str() << "\">\n";
	for(unsigned int i = 0; i < GDM_NORMAL; ++i)
	{
		if( m_TextureFileNames[i].length() > 0 )
			fout << "<" << m_MatNames[i] << "_Map>" << m_TextureFileNames[i].c_str() << "</" << m_MatNames[i] << "_Map>\n";

		fout << "<" << m_MatNames[i] << ">\n";
		fout << "<r>" << m_MaterialColors[i][0] << "</r>\n";
		fout << "<g>" << m_MaterialColors[i][1]<< "</g>\n";
		fout << "<b>" << m_MaterialColors[i][2] << "</b>\n";
		fout << "<a>" << m_MaterialColors[i][3]<< "</a>\n";
		fout << "</" << m_MatNames[i] << ">\n";
	}
	
	if( m_TextureFileNames[GDM_NORMAL].length() > 0 )
		fout << "<" << m_MatNames[GDM_NORMAL] << "_Map>" << m_TextureFileNames[GDM_NORMAL].c_str() << "</" << m_MatNames[GDM_NORMAL] << "_Map>\n";
	
	fout << "</GDMaterial>\n";
}

void GDMaterialExportCmd::ReplaceExtensionWithDDS(std::string &str)
{
	size_t len = str.length();

	if (len)
		str.replace(len-3, 3, "dds");
}