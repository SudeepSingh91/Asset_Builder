#ifndef EAE6320_CMAYAMESHEXPORTER_H
#define EAE6320_CMAYAMESHEXPORTER_H

#include "../../Engine/Windows/Includes.h"

#include <maya/MPxFileTranslator.h>

namespace eae6320
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
