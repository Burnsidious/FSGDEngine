#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include "GDExporter.h"
#include "GDMeshExportCmd.h"
#include "GDMaterialExportCmd.h"
#include "GDFormExportCmd.h"
#include "GDAnimationExportCmd.h"

// entry point
MStatus initializePlugin( MObject obj )
{
	MFnPlugin plugin(obj, "GDStudent", "1.0");
	
	//plugin.registerCommand("Foxporter", ModelExporter::creator, ModelExporter::newSyntax);
	
	//MGlobal::executeCommand("source \"FoxporterWindow.mel\"");
	//MGlobal::executeCommand("FoxporterWindow");

	//MStringArray exportPath;
	//MGlobal::executeCommand( "fileDialog2 -ds 2 -cap \"GDExporter Export Path\" -fm 3", exportPath );
	
	//if( exportPath.length() == 0 )
	//	return MS::kFailure;

	plugin.registerCommand("GDMeshExport", GDMeshExportCmd::creator, GDMeshExportCmd::newSyntax);
	plugin.registerCommand("GDMaterialExport", GDMaterialExportCmd::creator, GDMaterialExportCmd::newSyntax);
	plugin.registerCommand("GDFormExport", GDFormExportCmd::creator, GDFormExportCmd::newSyntax);
	plugin.registerCommand("GDAnimationExport", GDAnimationExportCmd::creator, GDAnimationExportCmd::newSyntax);

	return MS::kSuccess;
}

// exit point
MStatus uninitializePlugin( MObject obj )
{
	MFnPlugin pluginFn( obj );

	// If the UI window is up and running, delete it.
	//MGlobal::executeCommand("if ( `window -exists FoxporterWindow` ) deleteUI -window FoxporterWindow");
	
	// Deregister the command for the plugin.
	//pluginFn.deregisterCommand( "Foxporter" );
	pluginFn.deregisterCommand("GDMeshExport");
	pluginFn.deregisterCommand("GDMaterialExport");
	pluginFn.deregisterCommand("GDFormExport");
	pluginFn.deregisterCommand("GDModelExport");
	pluginFn.deregisterCommand("GDAnimationExport");

	GDExporter::DeleteInstance();

	return MS::kSuccess;
}