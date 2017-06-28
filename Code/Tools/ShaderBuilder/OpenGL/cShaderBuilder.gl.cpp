// Header Files
//=============

#include "../cShaderBuilder.h"

#include <cstdlib>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include "../../AssetBuildLibrary/UtilityFunctions.h"
#include "../../../Engine/Asserts/Asserts.h"
#include "../../../Engine/Graphics/OpenGL/Includes.h"
#include "../../../Engine/Platform/Platform.h"
#include "../../../Engine/Windows/OpenGl.h"
#include "../../../External/Mcpp/Includes.h"

// Static Data Initialization
//===========================

namespace
{
	namespace GlVendors
	{
		enum eGlVendor
		{
			NVIDIA,
			AMD,
			Intel,

			Other
		};
	}
	GlVendors::eGlVendor s_glVendor = GlVendors::Other;
}

// Helper Function Declarations
//=============================

namespace
{
	bool BuildAndVerifyGeneratedShaderSource( const char* const i_path_source, const char* const i_path_target,
		const eae6320::Graphics::ShaderTypes::eShaderType i_shaderType, const std::string& i_source );
	bool PreProcessShaderSource( const char* const i_path_source, std::string& o_shaderSource_preProcessed );
	bool SaveGeneratedShaderSource( const char* const i_path, const std::string& i_source );

	// This helper struct exists to be able to dynamically allocate memory to get "log info"
	// which will automatically be freed when the struct goes out of scope
	struct sLogInfo
	{
		GLchar* memory;
		sLogInfo( const size_t i_size ) { memory = reinterpret_cast<GLchar*>( malloc( i_size ) ); }
		~sLogInfo() { if ( memory ) free( memory ); }
	};
}

// Interface
//==========

// Build
//------

bool eae6320::AssetBuild::cShaderBuilder::Build( const Graphics::ShaderTypes::eShaderType i_shaderType, const std::vector<std::string>& i_arguments )
{
	bool wereThereErrors = false;

	std::string shaderSource_preProcessed;
	if ( !PreProcessShaderSource( m_path_source, shaderSource_preProcessed ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}
	if ( !SaveGeneratedShaderSource( m_path_target, shaderSource_preProcessed ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}
	if ( !BuildAndVerifyGeneratedShaderSource( m_path_source, m_path_target, i_shaderType, shaderSource_preProcessed ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}

OnExit:

	return !wereThereErrors;
}

// Helper Function Definitions
//============================

namespace
{
	bool BuildAndVerifyGeneratedShaderSource( const char* const i_path_source, const char* const i_path_target,
		const eae6320::Graphics::ShaderTypes::eShaderType i_shaderType, const std::string& i_source )
	{
		// Load any required OpenGL extensions
		{
			std::string errorMessage;
			if ( !eae6320::OpenGlExtensions::Load( &errorMessage ) )
			{
				eae6320::AssetBuild::OutputErrorMessage( errorMessage.c_str(), i_path_source );
				return false;
			}
		}

		bool wereThereErrors = false;

		// Create a hidden OpenGL window
		HINSTANCE hInstance = NULL;
		eae6320::Windows::OpenGl::sHiddenWindowInfo hiddenWindowInfo;
		{
			std::string errorMessage;
			if ( !eae6320::Windows::OpenGl::CreateHiddenContextWindow( hInstance, hiddenWindowInfo, &errorMessage ) )
			{
				wereThereErrors = true;
				eae6320::AssetBuild::OutputErrorMessage( errorMessage.c_str(), i_path_source );
				goto OnExit;
			}
		}
		// Determine which vendor makes the GPU
		{
			const GLubyte* const glString = glGetString( GL_VENDOR );
			const GLenum errorCode = glGetError();
			if ( glString && ( errorCode == GL_NO_ERROR ) )
			{
				const char* const glVendor = reinterpret_cast<const char*>( glString );
				if ( strcmp( glVendor, "NVIDIA Corporation" ) == 0 )
				{
					s_glVendor = GlVendors::NVIDIA;
				}
				else if ( strcmp( glVendor, "ATI Technologies Inc." ) == 0 )
				{
					s_glVendor = GlVendors::AMD;
				}
				else if ( strcmp( glVendor, "Intel" ) == 0 )
				{
					s_glVendor = GlVendors::Intel;
				}
				else
				{
					s_glVendor = GlVendors::Other;
				}
			}
			else
			{
				wereThereErrors = true;
				std::ostringstream errorMessage;
				errorMessage << "OpenGL failed to return a string identifying the GPU vendor";
				if ( errorCode != GL_NO_ERROR )
				{
					errorMessage << ": " << reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				}
				eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
				goto OnExit;
			}
		}
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv( GL_SHADER_COMPILER, &isShaderCompilingSupported );
			if ( !isShaderCompilingSupported )
			{
				eae6320::AssetBuild::OutputErrorMessage( "Compiling shaders at run-time isn't supported on this implementation"
					" (this should never happen)", i_path_source );
				return false;
			}
		}

		// Load the source code from file and set it into a shader
		GLuint shaderId = 0;
		{
			// Get the platform-specific shader type
			GLenum shaderType;
			switch ( i_shaderType )
			{
			case eae6320::Graphics::ShaderTypes::Vertex:
				shaderType = GL_VERTEX_SHADER;
				break;
			case eae6320::Graphics::ShaderTypes::Fragment:
				shaderType = GL_FRAGMENT_SHADER;
				break;
			default:
				wereThereErrors = true;
				std::ostringstream errorMessage;
				errorMessage << "No OpenGL implementation exists for the platform-independent shader type " << i_shaderType;
				eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
				goto OnExit;
			}
			// Generate a shader
			shaderId = glCreateShader( shaderType );
			{
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
						std::ostringstream errorMessage;
						errorMessage << "OpenGL failed to get an unused shader ID: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
					goto OnExit;
				}
				else if ( shaderId == 0 )
				{
					wereThereErrors = true;
					eae6320::AssetBuild::OutputErrorMessage( "OpenGL failed to get an unused shader ID", i_path_source );
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 1;
				const GLchar* const shaderSource = reinterpret_cast<const GLchar*>( i_source.c_str() );
				const GLint shaderLength = i_source.length();	// The NULL terminator isn't necessary
				glShaderSource( shaderId, shaderSourceCount, &shaderSource, &shaderLength );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					std::ostringstream errorMessage;
					errorMessage << "OpenGL failed to set the shader source code: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
					goto OnExit;
				}
			}
		}

		// Compile the shader source code
		{
			glCompileShader( shaderId );
			GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				// Get compilation info
				// (this won't be used unless compilation fails
				// but it can be useful to look at when debugging)
				std::string compilationInfo;
				{
					GLint infoSize;
					glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &infoSize );
					errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						sLogInfo info( static_cast<size_t>( infoSize ) );
						GLsizei* dontReturnLength = NULL;
						glGetShaderInfoLog( shaderId, static_cast<GLsizei>( infoSize ), dontReturnLength, info.memory );
						errorCode = glGetError();
						if ( errorCode == GL_NO_ERROR )
						{
							compilationInfo = info.memory;
						}
						else
						{
							wereThereErrors = true;
							std::ostringstream errorMessage;
							errorMessage << "OpenGL failed to get compilation info of the shader source code: " <<
								reinterpret_cast<const char*>( gluErrorString( errorCode ) );
							eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_target );
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::ostringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the shader compilation info: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_target );
						goto OnExit;
					}
				}
				// Check to see if there were compilation errors
				GLint didCompilationSucceed;
				{
					glGetShaderiv( shaderId, GL_COMPILE_STATUS, &didCompilationSucceed );
					errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						if ( didCompilationSucceed == GL_FALSE )
						{
							wereThereErrors = true;

							// Convert the error messages into something that will be output in Visual Studio's Error window
							// (and that will open the file when double-clicked)
							if ( s_glVendor != GlVendors::Other )
							{
								try
								{
									std::regex pattern_match;
									std::string pattern_replace;
									{
										switch ( s_glVendor )
										{
										case GlVendors::NVIDIA:
											{
												pattern_match.assign( R"(^\s*0\s*\(\s*(\d+)\s*\))" );
												{
													std::ostringstream stringBuilder;
													stringBuilder << i_path_target << "($1)";
													pattern_replace = stringBuilder.str();
												}
											}
											break;
										case GlVendors::AMD:
											{
												// The following code can be uncommented to try and debug AMD parsing on an NVIDIA machine
												/*
												{
													std::string fakeAmd;
													std::regex pattern_match( R"(^\s*0\s*\(\s*(\d+)\s*\)\s*:\s*(error|warning)\s*(\w+)\s*:)" );
													const std::string pattern_replace = "ERROR: 0:$1: $2(#$3)";
													fakeAmd = std::regex_replace( compilationInfo, pattern_match, pattern_replace );
													compilationInfo = fakeAmd;
												}
												*/

												pattern_match.assign( R"(^\s*\w+\s*:\s*0\s*:\s*(\d+)\s*:\s*(\w+)\s*\(\s*(\S+)\s*\))" );
												{
													std::ostringstream stringBuilder;
													stringBuilder << i_path_target << "($1) : $2 $3:";
													pattern_replace = stringBuilder.str();
												}
											}
											break;
										case GlVendors::Intel:
											{
												pattern_match.assign( R"(^\s*(\w+)\s*:\s*0\s*:\s*(\d+)\s*:)" );
												{
													std::ostringstream stringBuilder;
													stringBuilder << i_path_target << "($2) : $1:";
													pattern_replace = stringBuilder.str();
												}
											}
										}
									}
									const std::string convertedErrors = std::regex_replace( compilationInfo, pattern_match, pattern_replace );
									std::cerr << convertedErrors << "\n";
								}
								catch ( std::regex_error& )
								{
									// If the parsing code has an error pass the output directly from the driver
									std::cerr << compilationInfo << "\n";
								}
							}
							else
							{
								std::cerr << compilationInfo << "\n";
							}
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::ostringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the shader source code succeeded: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_target );
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				std::ostringstream errorMessage;
				errorMessage << "OpenGL failed to compile the shader source code: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_target );
				goto OnExit;
			}
		}

	OnExit:

		if ( shaderId != 0 )
		{
			glDeleteShader( shaderId );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				wereThereErrors = true;
				std::ostringstream errorMessage;
				errorMessage << "OpenGL failed to delete the shader source code: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
			}
			shaderId = 0;
		}

		{
			std::string errorMessage;
			if ( !eae6320::Windows::OpenGl::FreeHiddenContextWindow( hInstance, hiddenWindowInfo, &errorMessage ) )
			{
				wereThereErrors = true;
				eae6320::AssetBuild::OutputErrorMessage( errorMessage.c_str(), i_path_source );
			}
		}

		return !wereThereErrors;
	}

	bool PreProcessShaderSource( const char* const i_path_source, std::string& o_shaderSource_preProcessed )
	{
		bool wereThereErrors = false;

		// mcpp consumes non-const char*s
		// and so an array of temporary strings must be allocated
		const char* arguments_const [] =
		{
			// The command (necessary even though it is being run as a function)
			"mcpp",
			// The platform #define
			"-DEAE6320_PLATFORM_GL",
#ifdef EAE6320_GRAPHICS_AREDEBUGSHADERSENABLED
			// Keep comments
			"-C",
#endif
			// Don't output #line number information
			"-P",
			// Treat unknown directives (like #version and #extension) as warnings instead of errors
			"-a",
			// The input file to pre-process
			i_path_source
		};
		const size_t argumentCount = sizeof( arguments_const ) / sizeof( char* );
		char* arguments[argumentCount] = { NULL };
		for ( size_t i = 0; i < argumentCount; ++i )
		{
			const size_t stringSize = strlen( arguments_const[i] ) + 1;	// NULL terminator
			char* temporaryString = reinterpret_cast<char*>( malloc( stringSize ) );
			memcpy( temporaryString, arguments_const[i], stringSize );
			arguments[i] = temporaryString;
		}

		// Configure mcpp to output to buffers instead of files
		mcpp_use_mem_buffers( 1 );
		// Preprocess the file
		const int result = mcpp_lib_main( static_cast<int>( argumentCount ), arguments );
		if ( result == 0 )
		{
			o_shaderSource_preProcessed = mcpp_get_mem_buffer( static_cast<OUTDEST>( eae6320::mcpp::OUTDEST::Out ) );
		}
		else
		{
			wereThereErrors = true;
			std::cerr << mcpp_get_mem_buffer( static_cast<OUTDEST>( eae6320::mcpp::OUTDEST::Err ) );
			goto OnExit;
		}

#ifndef EAE6320_GRAPHICS_AREDEBUGSHADERSENABLED
		// Remove extra new lines
		try
		{
			std::regex pattern_match( R"(((\r\n)|(\n))+)" );
			const std::string pattern_replace( "\n" );
			o_shaderSource_preProcessed = std::regex_replace( o_shaderSource_preProcessed, pattern_match, pattern_replace );
		}
		catch ( std::regex_error& )
		{

		}
#endif

	OnExit:

		for ( size_t i = 0; i < argumentCount; ++i )
		{
			if ( arguments[i] )
			{
				free( arguments[i] );
				arguments[i] = NULL;
			}
		}

		// Release mcpp's internal buffers
		mcpp_use_mem_buffers( 0 );

		return !wereThereErrors;
	}

	bool SaveGeneratedShaderSource( const char* const i_path, const std::string& i_shader )
	{
		std::string errorMessage;
		if ( eae6320::Platform::WriteBinaryFile( i_path, i_shader.c_str(), i_shader.length(), &errorMessage ) )
		{
			return true;
		}
		else
		{
			eae6320::AssetBuild::OutputErrorMessage( errorMessage.c_str(), i_path );
			return false;
		}
	}
}
