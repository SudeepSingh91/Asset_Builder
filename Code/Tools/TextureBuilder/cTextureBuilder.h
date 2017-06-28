/*
	This class builds hardware-ready DDS textures from source images
*/

#ifndef EAE6320_CTEXTUREBUILDER_H
#define EAE6320_CTEXTUREBUILDER_H

// Header Files
//=============

#include "../AssetBuildLibrary/cbBuilder.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace AssetBuild
	{
		class cTextureBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			virtual bool Build( const std::vector<std::string>& i_arguments );
		};
	}
}

#endif	// EAE6320_CTEXTUREBUILDER_H
