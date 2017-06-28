#include "cMayaMeshExporter.h"
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>

namespace
{
	const char* s_pluginName = "Sudeep Singh EAE6320 Mesh Format";
}

__declspec(dllexport) MStatus initializePlugin( MObject io_object )
{
	MFnPlugin plugin( io_object );

	MStatus status;
	{
		char* noIcon = "none";
		status = plugin.registerFileTranslator( s_pluginName, noIcon,
			eae6320::cMayaMeshExporter::Create );
		if ( !status )
		{
			MGlobal::displayError( MString( "Failed to register mesh exporter: " ) + status.errorString() );
		}
	}
    return status;
}

__declspec(dllexport) MStatus uninitializePlugin( MObject io_object )
{
	MFnPlugin plugin( io_object );
	MStatus status;
	{
		status = plugin.deregisterFileTranslator( s_pluginName );
		if ( !status )
		{
			MGlobal::displayError( MString( "Failed to deregister mesh exporter: " ) + status.errorString() );
		}
	}
    return status;
}
