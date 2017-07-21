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

namespace
{
	bool BuildAndVerifyGeneratedShaderSource( const char* const i_path_source, const char* const i_path_target,
		const Engine::Graphics::ShaderTypes::eShaderType i_shaderType, const std::string& i_source );
	bool PreProcessShaderSource( const char* const i_path_source, std::string& o_shaderSource_preProcessed );
	bool SaveGeneratedShaderSource( const char* const i_path, const std::string& i_source );

	struct sLogInfo
	{
		GLchar* memory;
		sLogInfo( const size_t i_size ) { memory = reinterpret_cast<GLchar*>( malloc( i_size ) ); }
		~sLogInfo() { if ( memory ) free( memory ); }
	};
}

bool Engine::AssetBuild::cShaderBuilder::Build( const Graphics::ShaderTypes::eShaderType i_shaderType, const std::vector<std::string>& i_arguments )
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

namespace
{
	bool BuildAndVerifyGeneratedShaderSource( const char* const i_path_source, const char* const i_path_target,
		const Engine::Graphics::ShaderTypes::eShaderType i_shaderType, const std::string& i_source )
	{
		{
			std::string errorMessage;
			if ( !Engine::OpenGlExtensions::Load( &errorMessage ) )
			{
				Engine::AssetBuild::OutputErrorMessage( errorMessage.c_str(), i_path_source );
				return false;
			}
		}

		bool wereThereErrors = false;

		HINSTANCE hInstance = NULL;
		Engine::Windows::OpenGl::sHiddenWindowInfo hiddenWindowInfo;
		{
			std::string errorMessage;
			if ( !Engine::Windows::OpenGl::CreateHiddenContextWindow( hInstance, hiddenWindowInfo, &errorMessage ) )
			{
				wereThereErrors = true;
				Engine::AssetBuild::OutputErrorMessage( errorMessage.c_str(), i_path_source );
				goto OnExit;
			}
		}
		
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
				Engine::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
				goto OnExit;
			}
		}
		
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv( GL_SHADER_COMPILER, &isShaderCompilingSupported );
			if ( !isShaderCompilingSupported )
			{
				Engine::AssetBuild::OutputErrorMessage( "Compiling shaders at run-time isn't supported on this implementation"
					" (this should never happen)", i_path_source );
				return false;
			}
		}

		GLuint shaderId = 0;
		{
			GLenum shaderType;
			switch ( i_shaderType )
			{
			case Engine::Graphics::ShaderTypes::Vertex:
				shaderType = GL_VERTEX_SHADER;
				break;
			case Engine::Graphics::ShaderTypes::Fragment:
				shaderType = GL_FRAGMENT_SHADER;
				break;
			default:
				wereThereErrors = true;
				std::ostringstream errorMessage;
				errorMessage << "No OpenGL implementation exists for the platform-independent shader type " << i_shaderType;
				Engine::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
				goto OnExit;
			}
			
			shaderId = glCreateShader( shaderType );
			{
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
						std::ostringstream errorMessage;
						errorMessage << "OpenGL failed to get an unused shader ID: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					Engine::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
					goto OnExit;
				}
				else if ( shaderId == 0 )
				{
					wereThereErrors = true;
					Engine::AssetBuild::OutputErrorMessage( "OpenGL failed to get an unused shader ID", i_path_source );
					goto OnExit;
				}
			}
			
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
					Engine::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
					goto OnExit;
				}
			}
		}

		{
			glCompileShader( shaderId );
			GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
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
							Engine::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_target );
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::ostringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the shader compilation info: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						Engine::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_target );
						goto OnExit;
					}
				}
				
				GLint didCompilationSucceed;
				{
					glGetShaderiv( shaderId, GL_COMPILE_STATUS, &didCompilationSucceed );
					errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						if ( didCompilationSucceed == GL_FALSE )
						{
							wereThereErrors = true;

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
						Engine::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_target );
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
				Engine::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_target );
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
				Engine::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), i_path_source );
			}
			shaderId = 0;
		}

		{
			std::string errorMessage;
			if ( !Engine::Windows::OpenGl::FreeHiddenContextWindow( hInstance, hiddenWindowInfo, &errorMessage ) )
			{
				wereThereErrors = true;
				Engine::AssetBuild::OutputErrorMessage( errorMessage.c_str(), i_path_source );
			}
		}

		return !wereThereErrors;
	}

	bool PreProcessShaderSource( const char* const i_path_source, std::string& o_shaderSource_preProcessed )
	{
		bool wereThereErrors = false;
		const char* arguments_const [] =
		{
			"mcpp",
			"-DPLATFORM_GL",
#ifdef GRAPHICS_AREDEBUGSHADERSENABLED
			"-C",
#endif
			"-P",
			"-a",
			i_path_source
		};
		const size_t argumentCount = sizeof( arguments_const ) / sizeof( char* );
		char* arguments[argumentCount] = { NULL };
		for ( size_t i = 0; i < argumentCount; ++i )
		{
			const size_t stringSize = strlen( arguments_const[i] ) + 1;	
			char* temporaryString = reinterpret_cast<char*>( malloc( stringSize ) );
			memcpy( temporaryString, arguments_const[i], stringSize );
			arguments[i] = temporaryString;
		}

		mcpp_use_mem_buffers( 1 );
		const int result = mcpp_lib_main( static_cast<int>( argumentCount ), arguments );
		if ( result == 0 )
		{
			o_shaderSource_preProcessed = mcpp_get_mem_buffer( static_cast<OUTDEST>( Engine::mcpp::OUTDEST::Out ) );
		}
		else
		{
			wereThereErrors = true;
			std::cerr << mcpp_get_mem_buffer( static_cast<OUTDEST>( Engine::mcpp::OUTDEST::Err ) );
			goto OnExit;
		}

#ifndef GRAPHICS_AREDEBUGSHADERSENABLED
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

		mcpp_use_mem_buffers( 0 );

		return !wereThereErrors;
	}

	bool SaveGeneratedShaderSource( const char* const i_path, const std::string& i_shader )
	{
		std::string errorMessage;
		if ( Engine::Platform::WriteBinaryFile( i_path, i_shader.c_str(), i_shader.length(), &errorMessage ) )
		{
			return true;
		}
		else
		{
			Engine::AssetBuild::OutputErrorMessage( errorMessage.c_str(), i_path );
			return false;
		}
	}
}
