// Header Files
//=============

#include "../OpenGlExtensions.h"

#include <sstream>
#include "../../../Engine/Asserts/Asserts.h"
#include "../../../Engine/Windows/Functions.h"
#include "../../../Engine/Windows/OpenGl.h"

// Helper Function Declarations
//=============================

namespace
{
	void* GetGlFunctionAddress( const char* i_functionName, std::string* o_errorMessage = NULL );
}

// Interface
//==========

// OpenGL Extension Definitions
//-----------------------------

PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBINDBUFFERBASEPROC glBindBufferBase = NULL;
PFNGLBINDSAMPLERPROC glBindSampler = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
PFNGLBLENDEQUATIONPROC glBlendEquation = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLDELETESAMPLERSPROC glDeleteSamplers = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArray = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLGENSAMPLERSPROC glGenSamplers = NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLINVALIDATEBUFFERDATAPROC glInvalidateBufferData = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLUNIFORM1FVPROC glUniform1fv = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;
PFNGLUNIFORM2FVPROC glUniform2fv = NULL;
PFNGLUNIFORM3FVPROC glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

// Initialization
//---------------

bool eae6320::OpenGlExtensions::Load( std::string* o_errorMessage )
{
	bool wereThereErrors = false;

	// Load a hidden window
	// (The Windows OpenGL implementation requires an OpenGL window to be created
	// before any of the functions can be called,
	// and so we create a hidden window in order to load the extensions
	// and then destroy it afterwards,
	// all before doing anything with the main window)
	HINSTANCE hInstance = NULL;
	eae6320::Windows::OpenGl::sHiddenWindowInfo hiddenWindowInfo;
	if ( eae6320::Windows::OpenGl::CreateHiddenContextWindow( hInstance, hiddenWindowInfo, o_errorMessage ) )
	{
		EAE6320_ASSERTF( wglGetCurrentContext() != NULL, "OpenGL extensions can't be loaded without a current OpenGL context" );
	}
	else
	{
		wereThereErrors = true;
		EAE6320_ASSERTF( false, o_errorMessage->c_str() );
		goto OnExit;
	}

	// Load each extension
#define EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( i_functionName, i_functionType )														\
		i_functionName = reinterpret_cast<i_functionType>( GetGlFunctionAddress( #i_functionName, o_errorMessage ) );	\
		if ( !i_functionName )																							\
			goto OnExit;

	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glActiveTexture, PFNGLACTIVETEXTUREPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glAttachShader, PFNGLATTACHSHADERPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glBindBuffer, PFNGLBINDBUFFERPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glBindBufferBase, PFNGLBINDBUFFERBASEPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glBindSampler, PFNGLBINDSAMPLERPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glBindVertexArray, PFNGLBINDVERTEXARRAYPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glBlendEquation, PFNGLBLENDEQUATIONPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glBufferData, PFNGLBUFFERDATAPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glBufferSubData, PFNGLBUFFERSUBDATAPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glCompileShader, PFNGLCOMPILESHADERPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glCompressedTexImage2D, PFNGLCOMPRESSEDTEXIMAGE2DPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glCreateProgram, PFNGLCREATEPROGRAMPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glCreateShader, PFNGLCREATESHADERPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glDeleteBuffers, PFNGLDELETEBUFFERSPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glDeleteProgram, PFNGLDELETEPROGRAMPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glDeleteSamplers, PFNGLDELETESAMPLERSPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glDeleteShader, PFNGLDELETESHADERPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYARBPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glGenBuffers, PFNGLGENBUFFERSPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glGenSamplers, PFNGLGENSAMPLERSPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glGetProgramiv, PFNGLGETPROGRAMIVPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glGetShaderiv, PFNGLGETSHADERIVPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glInvalidateBufferData, PFNGLINVALIDATEBUFFERDATAPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glLinkProgram, PFNGLLINKPROGRAMPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glSamplerParameteri, PFNGLSAMPLERPARAMETERIPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glShaderSource, PFNGLSHADERSOURCEPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glUniform1fv, PFNGLUNIFORM1FVPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glUniform1i, PFNGLUNIFORM1IPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glUniform2fv, PFNGLUNIFORM2FVPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glUniform3fv, PFNGLUNIFORM3FVPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glUniform4fv, PFNGLUNIFORM4FVPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glUniformBlockBinding, PFNGLUNIFORMBLOCKBINDINGPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glUseProgram, PFNGLUSEPROGRAMPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC );
	EAE6320_OPENGLEXTENSIONS_LOADFUNCTION( wglCreateContextAttribsARB, PFNWGLCREATECONTEXTATTRIBSARBPROC );

#undef EAE6320_OPENGLEXTENSIONS_LOADFUNCTION

OnExit:

	if ( !eae6320::Windows::OpenGl::FreeHiddenContextWindow( hInstance, hiddenWindowInfo, o_errorMessage ) )
	{
		wereThereErrors = true;
		EAE6320_ASSERTF( false, o_errorMessage->c_str() );
	}

	return !wereThereErrors;
}

// Helper Function Declarations
//=============================

namespace
{
	void* GetGlFunctionAddress( const char* i_functionName, std::string* o_errorMessage )
	{
		void* address = reinterpret_cast<void*>( wglGetProcAddress( i_functionName ) );
		// The documentation says that NULL will be returned if the function isn't found,
		// but according to https://www.opengl.org/wiki/Load_OpenGL_Functions
		// other values can be returned by some implementations
		if ( ( address != NULL )
			&& ( address != reinterpret_cast<void*>( 1 ) ) && ( address != reinterpret_cast<void*>( 2 ) )
			&& ( address != reinterpret_cast<void*>( 3 ) ) && ( address != reinterpret_cast<void*>( -1 ) ) )
		{
			return address;
		}
		std::string wglErrorMessage;
		if ( address == NULL )
		{
			wglErrorMessage = eae6320::Windows::GetLastSystemError();
			EAE6320_ASSERTF( false, "The OpenGL extension function \"%s\" wasn't found"
				" (it will now be looked for in the non-extension Windows functions)", i_functionName );
		}
		// wglGetProcAddress() won't return the address of any 1.1 or earlier OpenGL functions
		// that are built into Windows' Opengl32.dll
		// but an attempt can be made to load those manually
		// in case the user of this function has made a mistake
		{
			// This library should already be loaded,
			// and so this function will just retrieve a handle to it
			HMODULE glLibrary = LoadLibrary( "Opengl32.dll" );
			if ( glLibrary != NULL )
			{
				// Look for an old OpenGL function
				void* address = reinterpret_cast<void*>( GetProcAddress( glLibrary, i_functionName ) );
				// Decrement the library's reference count
				FreeLibrary( glLibrary );
				// Return an address if it was found
				if ( address != NULL )
				{
					return address;
				}
				else
				{
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
					if ( wglErrorMessage.empty() )
					{
						wglErrorMessage = windowsErrorMessage;
					}
				}
			}
			else
			{
				EAE6320_ASSERT( false );
			}
		}

		// If this code is reached the OpenGL function wasn't found
		if ( o_errorMessage )
		{
			std::ostringstream errorMessage;
			errorMessage << "Windows failed to find the address of the OpenGL function \"" << i_functionName << "\"";
			if ( !wglErrorMessage.empty() )
			{
				errorMessage << ": " << wglErrorMessage;
			}
			*o_errorMessage = errorMessage.str();
		}

		return NULL;
	}
}
