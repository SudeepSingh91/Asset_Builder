#ifndef EAE6320_BUILDERHELPER_CBBUILDER_H
#define EAE6320_BUILDERHELPER_CBBUILDER_H

#include <cstdlib>
#include <string>
#include <vector>

namespace eae6320
{
	namespace AssetBuild
	{
		template<class tBuilder>
		int Build( char** i_arguments, const unsigned int i_argumentCount )
		{
			tBuilder builder;
			return builder.ParseCommandArgumentsAndBuild( i_arguments, i_argumentCount ) ?
				EXIT_SUCCESS : EXIT_FAILURE;
		}

		class cbBuilder
		{
		public:
			bool ParseCommandArgumentsAndBuild( char** i_arguments, const unsigned int i_argumentCount );
			cbBuilder();

		protected:
			const char* m_path_source;
			const char* m_path_target;

		private:
			virtual bool Build( const std::vector<std::string>& i_optionalArguments ) = 0;
		};
	}
}

#endif
