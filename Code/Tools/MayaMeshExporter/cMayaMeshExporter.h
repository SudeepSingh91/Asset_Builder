#ifndef CMAYAMESHEXPORTER_H
#define CMAYAMESHEXPORTER_H

#include "../../Engine/Windows/Includes.h"

#include <maya/MPxFileTranslator.h>

namespace Engine
{
	class cMayaMeshExporter : public MPxFileTranslator
	{
	public:
		virtual bool haveWriteMethod() const { return true; }
		virtual MStatus writer( const MFileObject& i_file, const MString& i_options, FileAccessMode i_mode );
		virtual MString defaultExtension() const { return ".lua"; }

	public:
		static void* Create()
		{
			return new cMayaMeshExporter;
		}
	};
}

#endif
