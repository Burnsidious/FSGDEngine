#include "GDFormExportCmd.h"
#include "GDExporter.h"
#include "GDRenderContext.h"

using namespace std;

void* GDFormExportCmd::creator(void)
{
	return new GDFormExportCmd;
}

MSyntax GDFormExportCmd::newSyntax(void)
{
	MSyntax syntax;
	return syntax;
}

MStatus GDFormExportCmd::doIt(const MArgList &args)
{
	Reset();

	const char* exportPath = GDExporter::GetInstance()->EstablishExportPath();

	if( exportPath == 0 )
	{
		MGlobal::displayError(MString("GDMeshExport - No export path established."));
		return MS::kFailure;
	}

	std::list<GDRenderContext> uniqueContexts;
	
	MDagPath selectedTransformPath;
	if( GDExporter::GetInstance()->GetSelectedMeshTransformPath(selectedTransformPath) )
	{
		MFnTransform selectedTransform(selectedTransformPath);
		GDExporter::GetInstance()->GetUniqueRenderContexts( selectedTransform, uniqueContexts );

		std::list<GDRenderContext>::iterator rcxIter = uniqueContexts.begin();
		for(; rcxIter != uniqueContexts.end(); ++rcxIter)
			(*rcxIter).WriteXML();

		ExportForms( selectedTransform );
		return MS::kSuccess;
	}
	
	return MS::kFailure;
	//MDagPath meshPath;
	//exportingMesh->getPath(meshPath);

	//MFnDependencyNode depNode(meshPath.node());
	//MStatus fpResult;
	//MPlug gdMaterialPlug = exportingMesh->findPlug("GDMaterial", &fpResult);

	//bool meshHasMaterial = true;
	//if( fpResult != MS::kSuccess )
	//	meshHasMaterial = false;

	//MPlug gdMeshPlug = exportingMesh->findPlug("GDMesh", &fpResult);

	//if( fpResult != MS::kSuccess )
	//{
	//	MGlobal::displayError(MString("GDFormExport - Selection missing associated GDMesh."));
	//	delete exportingMesh;
	//	return MS::kFailure;
	//}

	//MPlug gdRenderContextPlug = exportingMesh->findPlug("GDRenderContext", &fpResult);

	//if( fpResult != MS::kSuccess )
	//{
	//	MStringArray renderContextFile;
	//	std::string commandString = "fileDialog2 -ds 2 -cap \"Select RenderContext File\" -fm 1 -ff \"*.xml\" -dir ";
	//	commandString += "\"";
	//	commandString += exportPath;
	//	commandString += "\"";

	//	MGlobal::executeCommand( commandString.c_str(), renderContextFile );

	//	if( renderContextFile.length() != 0 )
	//	{
	//		MStatus result;
	//		MFnTypedAttribute typedAttr;
	//		MObject newAttribute= typedAttr.create( "GDRenderContext", "GDRcx", MFnData::kString, &result);
	//		typedAttr.setHidden(false);
	//		exportingMesh->addAttribute( newAttribute );

	//		gdRenderContextPlug = exportingMesh->findPlug("GDRenderContext", &fpResult);

	//		if( fpResult == MS::kSuccess )
	//		{
	//			MStringArray splitFilePath;
	//			renderContextFile[0].split( '/', splitFilePath );
	//			std::string rcString = "/GDRenderContext/";
	//			rcString += splitFilePath[ splitFilePath.length() - 1 ].asChar();
	//			gdRenderContextPlug.setValue( rcString.c_str() );
	//		}
	//	}
	//}

	//if( fpResult != MS::kSuccess )
	//{
	//	MGlobal::displayError(MString("GDFormExport - Selection missing associated GDRenderContext."));		
	//	delete exportingMesh;
	//	return MS::kFailure;
	//}
	//
	//MString plugVal;
	//gdMeshPlug.getValue(plugVal);
	//m_Form.m_Mesh = plugVal.asChar();
	//
	//if(meshHasMaterial)
	//{
	//	gdMaterialPlug.getValue(plugVal);
	//	m_Form.m_Material = plugVal.asChar();
	//}

	//gdRenderContextPlug.child(0).getValue(plugVal);
	//m_Form.m_RenderContext = plugVal.asChar();

	//MStringArray pathStepNames;
	//exportingMesh->fullPathName().split('|', pathStepNames);

	//std::string relativePath;
	//std::string filePath = exportPath;
	//filePath += "/GDForm";
	//relativePath += "/GDForm";
	//_mkdir( filePath.c_str() );

	//std::string sceneName;
	//GDExporter::GetInstance()->GetSceneName(sceneName);
	//filePath += "/";
	//relativePath += "/";
	//filePath += sceneName.c_str();
	//relativePath += sceneName.c_str();

	//_mkdir( filePath.c_str() );

	//for(unsigned int i = 0; i < pathStepNames.length(); ++i)
	//{
	//	relativePath += "/";
	//	filePath += "/";
	//	relativePath += pathStepNames[i].asChar();
	//	filePath += pathStepNames[i].asChar();
	//	_mkdir( filePath.c_str() );		
	//}

	//std::string::size_type slashPos = m_Form.m_Mesh.find_last_of('/');
	//std::string::size_type dotPos = m_Form.m_Mesh.find_first_of('.');
	//if( slashPos == std::string::npos )
	//	slashPos = 0;
	//else
	//	slashPos += 1;

	//if( dotPos == std::string::npos )
	//	dotPos = m_Form.m_Mesh.length();

	//m_Form.m_FormName += m_Form.m_Mesh.substr( slashPos, dotPos - slashPos );
	//m_Form.m_FormName += "_";

	//if( m_Form.m_Material.length() != 0 )
	//{
	//	slashPos = m_Form.m_Material.find_last_of('/');
	//	dotPos = m_Form.m_Material.find_first_of('.');
	//	if( slashPos == std::string::npos )
	//		slashPos = 0;
	//	else
	//		slashPos += 1;

	//	if( dotPos == std::string::npos )
	//		dotPos = m_Form.m_Material.length();

	//	m_Form.m_FormName += m_Form.m_Material.substr( slashPos, dotPos - slashPos );
	//	m_Form.m_FormName += "_";
	//}

	//slashPos = m_Form.m_RenderContext.find_last_of('/');
	//dotPos = m_Form.m_RenderContext.find_first_of('.');
	//if( slashPos == std::string::npos )
	//	slashPos = 0;
	//else
	//	slashPos += 1;

	//if( dotPos == std::string::npos )
	//	dotPos = m_Form.m_RenderContext.length();

	//m_Form.m_FormName += "rc_";
	//m_Form.m_FormName += m_Form.m_RenderContext.substr( slashPos, dotPos - slashPos );

	//std::string fileName = "frm_";
	//fileName += m_Form.m_FormName;
	//fileName += ".xml";

	//filePath += "/";
	//filePath += fileName;

	//relativePath += "/";
	//relativePath += fileName;

	//if( !GDExporter::GetInstance()->ValidateFileCreation( filePath.c_str() ) )
	//{
	//	MGlobal::displayInfo( exportingMesh->partialPathName() + " export canceled." );
	//	delete exportingMesh;
	//	return MS::kFailure;
	//}

	//ofstream fout( filePath.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
	//m_Form.WriteXML(fout);
	//fout.close();

	//MPlug gdFormPlug = exportingMesh->findPlug("GDForm", &fpResult);
	//
	//if( fpResult != MS::kSuccess )
	//{
	//	MStatus result;
	//	MFnTypedAttribute typedAttr;
	//	MObject newAttribute= typedAttr.create( "GDForm", "GDFrm", MFnData::kString, &result);
	//	typedAttr.setHidden(false);
	//	exportingMesh->addAttribute( newAttribute );

	//	gdFormPlug = exportingMesh->findPlug("GDForm", &fpResult);
	//}

	//if( fpResult == MS::kSuccess )
	//	gdFormPlug.setValue(relativePath.c_str());

	//delete exportingMesh;
	//return MS::kSuccess;
}

void GDForm::WriteXML(std::ofstream &fout)
{
	fout << "<?xml version=\"1.0\"?>\n";
	fout << "<GDForm name=\"" << m_FormName.c_str() << "\">\n";
	fout << "<GDMeshFile>" << m_Mesh.c_str() << "</GDMeshFile>\n";
	if( m_Material.length() != 0 )
		fout << "<GDMaterialFile>" << m_Material.c_str() << "</GDMaterialFile>\n";
	fout << "<GDRenderContextFile>" << m_RenderContext.c_str() << "</GDRenderContextFile>\n";
	fout << "</GDForm>\n";
}

void GDFormExportCmd::ExportForms(MFnTransform &transform)
{
	for(unsigned int i = 0; i < transform.childCount(); ++i)
	{
		if( transform.child(i).apiType() == MFn::kMesh )
		{
			MFnMesh childMesh( transform.child(i) );

			if( childMesh.isIntermediateObject() )
				continue;

			std::list<MDagPath>::iterator uniquePathIter = m_ExportedPaths.begin();
			MDagPath childPath;
			childMesh.getPath(childPath);
	
			bool bExported = false;
			for(; uniquePathIter != m_ExportedPaths.end(); ++uniquePathIter)
			{
				if( (*uniquePathIter) == childPath )
				{
					bExported = true;
					break;
				}
			}

			if( bExported )
				continue;
			else
				m_ExportedPaths.push_back(childPath);

			MStatus status;
			MPlug gdMeshPlug = childMesh.findPlug("GDMesh", &status);

			if( status != MS::kSuccess )
				continue;

			MString meshString;
			gdMeshPlug.getValue(meshString);

			if( meshString.length() == 0 )
				continue;

			MPlug gdMaterialPlug = childMesh.findPlug("GDMaterial", &status);
			MString materialName;
			if( status == MS::kSuccess )
				gdMaterialPlug.getValue(materialName);

			MPlug gdRenderContextPlug = childMesh.findPlug("GDRenderContext", &status);
			if( status != MS::kSuccess )
				continue;

			MString rcString;
			gdRenderContextPlug.child(0).getValue(rcString);
			if( rcString.length() == 0 )
				continue;

			MPlug formPlug = childMesh.findPlug("GDForm", &status);

			if( status != MS::kSuccess )
			{
				MFnTypedAttribute formAttr;
				MObject formAttrObj = formAttr.create( "GDForm", "GDFrm", MFnData::kString, &status);
				childMesh.addAttribute( formAttrObj );
				formPlug = childMesh.findPlug("GDForm", &status);
			}

			if( status != MS::kSuccess )
				continue;
			
			const char* exportPath = GDExporter::GetInstance()->EstablishExportPath();

			GDForm currentForm;

			if( materialName.length() != 0 )
				currentForm.m_Material = materialName.asChar();

			currentForm.m_Mesh = meshString.asChar();
			currentForm.m_RenderContext = rcString.asChar();

			MStringArray pathStepNames;
			childMesh.fullPathName().split('|', pathStepNames);

			std::string relativePath;
			std::string filePath = exportPath;
			filePath += "/GDForm";
			relativePath += "/GDForm";
			_mkdir( filePath.c_str() );

			std::string sceneName;
			GDExporter::GetInstance()->GetSceneName(sceneName);
			filePath += "/";
			relativePath += "/";
			filePath += sceneName.c_str();
			relativePath += sceneName.c_str();

			_mkdir( filePath.c_str() );

			for(unsigned int i = 0; i < pathStepNames.length(); ++i)
			{
				relativePath += "/";
				filePath += "/";
				relativePath += pathStepNames[i].asChar();
				filePath += pathStepNames[i].asChar();
				_mkdir( filePath.c_str() );		
			}

			std::string::size_type slashPos = currentForm.m_Mesh.find_last_of('/');
			std::string::size_type dotPos = currentForm.m_Mesh.find_first_of('.');
			if( slashPos == std::string::npos )
				slashPos = 0;
			else
				slashPos += 1;

			if( dotPos == std::string::npos )
				dotPos = currentForm.m_Mesh.length();

			//currentForm.m_FormName += currentForm.m_Mesh.substr( slashPos, dotPos - slashPos );
			//currentForm.m_FormName += "_";

			if( currentForm.m_Material.length() != 0 )
			{
				slashPos = currentForm.m_Material.find_last_of('/');
				dotPos = currentForm.m_Material.find_first_of('.');
				if( slashPos == std::string::npos )
					slashPos = 0;
				else
					slashPos += 1;

				if( dotPos == std::string::npos )
					dotPos = currentForm.m_Material.length();

				currentForm.m_FormName += currentForm.m_Material.substr( slashPos, dotPos - slashPos );
				currentForm.m_FormName += "_";
			}

			slashPos = currentForm.m_RenderContext.find_last_of('/');
			dotPos = currentForm.m_RenderContext.find_first_of('.');
			if( slashPos == std::string::npos )
				slashPos = 0;
			else
				slashPos += 1;

			if( dotPos == std::string::npos )
				dotPos = currentForm.m_RenderContext.length();

			currentForm.m_FormName += "rc_";
			currentForm.m_FormName += currentForm.m_RenderContext.substr( slashPos, dotPos - slashPos );

			std::string fileName = "frm_";
			fileName += currentForm.m_FormName;
			fileName += ".xml";

			filePath += "/";
			filePath += fileName;

			relativePath += "/";
			relativePath += fileName;

			if( !GDExporter::GetInstance()->ValidateFileCreation( filePath.c_str() ) )
			{
				MGlobal::displayInfo( childMesh.partialPathName() + " export canceled." );
				continue;
			}

			formPlug.setValue(relativePath.c_str());
			//MFnTypedAttribute formAttr(formPlug);
			//formAttr.setWritable(false);

			ofstream fout( filePath.c_str(), ios_base::out | ios_base::binary | ios_base::trunc );
			currentForm.WriteXML(fout);
			fout.close();
			
		}
		else if( transform.child(i).apiType() == MFn::kTransform )
		{
			MFnTransform childTransform( transform.child(i) );
			ExportForms(childTransform);
		}
	}
}