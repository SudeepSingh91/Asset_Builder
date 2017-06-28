#ifndef EAE6320_MATERIALBUILDER_H
#define EAE6320_MATERIALBUILDER_H

// Header Files
//=============

#include "../AssetBuildLibrary/cbBuilder.h"
#include "../../External/Lua/Includes.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace AssetBuild
	{
		class cMaterialBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			virtual bool Build(const std::vector<std::string>& i_arguments);
		};
	}
}

#endif	// EAE6320_CMATERIALBUILDER_H

