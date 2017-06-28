#include "../Graphics.h"
#include "../Mesh.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include "Includes.h"
#include <string>
#include <sstream>
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../Platform/Platform.h"
#include "../../Time/Time.h"
#include "../../Windows/Functions.h"
#include "../../Windows/OpenGl.h"
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"

namespace
{
	HWND s_renderingWindow = NULL;
	HDC s_deviceContext = NULL;
	HGLRC s_openGlRenderingContext = NULL;
	GLuint s_samplerState = NULL;
	GLuint s_vertexArrayId = 0;
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
	GLuint s_vertexBufferId = 0;
#endif
	GLuint s_programId = 0;
	eae6320::ConstantBuffer::sFrame s_Frame;
	eae6320::ConstantBuffer::sDrawCall s_Draw;

	eae6320::ConstantBuffer::ConstantBuffer s_FrameBuffer;
	eae6320::ConstantBuffer::ConstantBuffer s_DrawBuffer;

	eae6320::Math::cVector camPos(0.0f, 1.5f, 10.0f);
	eae6320::Math::cQuaternion camOri;
	eae6320::Camera::Camera s_Camera(camPos, camOri, eae6320::Math::ConvertDegreesToRadians(60.0f), 0.1f, 100.0f);

	std::vector<eae6320::Graphics::DrawCallData> s_MeshList;
	std::vector<eae6320::Graphics::SpriteCallData> s_SprList;
}

namespace
{
	bool CreateConstantBuffer();
	bool CreateProgram();
	bool CreateRenderingContext();
	bool EnableBackfaceCulling();
	bool CreateVertexBuffer();
	bool LoadAndAllocateShaderProgram( const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage );
	bool CreateSampler();

	struct sLogInfo
	{
		GLchar* memory;
		sLogInfo( const size_t i_size ) { memory = reinterpret_cast<GLchar*>( malloc( i_size ) ); }
		~sLogInfo() { if ( memory ) free( memory ); }
	};
}

void eae6320::Graphics::RenderFrame()
{
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		const GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
		glClear(clearColor);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		glDepthMask(GL_TRUE);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		glClearDepth(1.0f);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		const GLbitfield clearColorAndDepth = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		glClear(clearColorAndDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}

	{
		s_Frame.g_transform_worldToCamera = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(s_Camera.Ori(), s_Camera.Pos());
		s_Frame.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
		s_FrameBuffer.Update(reinterpret_cast<void*>(&s_Frame));
	}

	for (std::vector<eae6320::Graphics::DrawCallData>::iterator it = s_MeshList.begin(); it != s_MeshList.end(); ++it)
	{
		it->s_mat->Bind();
		s_Draw = it->s_drawCall;
		s_DrawBuffer.Update(reinterpret_cast<void*>(&s_Draw));
		it->s_mesh->Draw();
	}
	s_MeshList.clear();

	for (std::vector<eae6320::Graphics::SpriteCallData>::iterator it = s_SprList.begin(); it != s_SprList.end(); ++it)
	{
		it->s_mat->Bind();
		it->s_sprite->Draw();
	}
	s_SprList.clear();
	{
		BOOL result = SwapBuffers( s_deviceContext );
		EAE6320_ASSERT( result != FALSE );
	}
}

void eae6320::Graphics::SetMesh(eae6320::Graphics::MeshData i_mesh)
{
	eae6320::Math::cVector temp1(0.0f, 1.0f, 0.0f);
	eae6320::Math::cQuaternion tempRot(eae6320::Math::ConvertDegreesToRadians(i_mesh.s_rot.y), temp1);
	eae6320::Math::cMatrix_transformation tempMat(tempRot, i_mesh.s_pos);
	eae6320::ConstantBuffer::sDrawCall tempDraw;
	tempDraw.g_transform_localToWorld = tempMat;
	eae6320::Graphics::DrawCallData s_data;
	s_data.s_mesh = i_mesh.s_mesh;
	s_data.s_mat = i_mesh.s_mat;
	s_data.s_drawCall = tempDraw;
	s_MeshList.push_back(s_data);
}

void eae6320::Graphics::SetSprite(cSprite* i_spr, Materials::Material* i_mat)
{
	SpriteCallData temp;
	temp.s_sprite = i_spr;
	temp.s_mat = i_mat;
	s_SprList.push_back(temp);
}

void eae6320::Graphics::SetCamera(eae6320::Camera::Camera i_cam)
{
	s_Camera = i_cam;
}

bool eae6320::Graphics::Initialize( const sInitializationParameters& i_initializationParameters )
{
	std::string errorMessage;

	s_renderingWindow = i_initializationParameters.mainWindow;
	if ( !OpenGlExtensions::Load( &errorMessage ) )
	{
		EAE6320_ASSERTF( false, errorMessage.c_str() );
		Logging::OutputError( errorMessage.c_str() );
		return false;
	}
	if ( !CreateRenderingContext() )
	{
		EAE6320_ASSERT( false );
		return false;
	}

	if (!CreateSampler())
	{
		EAE6320_ASSERT(false);
		return false;
	}

	if ( !CreateConstantBuffer())
	{
		EAE6320_ASSERT( false );
		return false;
	}

	return true;
}

bool eae6320::Graphics::CleanUp()
{
	bool wereThereErrors = false;

	if ( s_openGlRenderingContext != NULL )
	{

		if (!s_FrameBuffer.CleanUp())
			wereThereErrors = true;
		if (!s_DrawBuffer.CleanUp())
			wereThereErrors = true;

		if ( wglMakeCurrent( s_deviceContext, NULL ) != FALSE )
		{
			if ( wglDeleteContext( s_openGlRenderingContext ) == FALSE )
			{
				wereThereErrors = true;
				const std::string windowsErrorMessage = Windows::GetLastSystemError();
				EAE6320_ASSERTF( false, windowsErrorMessage.c_str() );
				Logging::OutputError( "Windows failed to delete the OpenGL rendering context: %s", windowsErrorMessage.c_str() );
			}
		}
		else
		{
			wereThereErrors = true;
			const std::string windowsErrorMessage = Windows::GetLastSystemError();
			EAE6320_ASSERTF( false, windowsErrorMessage.c_str() );
			Logging::OutputError( "Windows failed to unset the current OpenGL rendering context: %s", windowsErrorMessage.c_str() );
		}
		s_openGlRenderingContext = NULL;
	}

	if (s_samplerState != NULL)
	{
		GLsizei count = 1;
		glDeleteSamplers(count, &s_samplerState);
	}

	if ( s_deviceContext != NULL )
	{
		ReleaseDC( s_renderingWindow, s_deviceContext );
		s_deviceContext = NULL;
	}

	s_renderingWindow = NULL;

	return !wereThereErrors;
}

namespace
{
	bool CreateConstantBuffer()
	{
		bool wereThereErrors = false;
		GLint m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		s_Frame.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
		s_Frame.g_transform_worldToCamera = eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform(s_Camera.Ori(), s_Camera.Pos());
		s_Frame.g_transform_cameraToScreen = eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform(s_Camera.fov(), static_cast<float>(m_viewport[2]/m_viewport[3]), s_Camera.nearPlaneDist(), s_Camera.farPlaneDist());

		if (!s_FrameBuffer.Initialize(eae6320::ConstantBuffer::sBufferType::frameType, sizeof(eae6320::ConstantBuffer::sFrame), reinterpret_cast<void*>(&s_Frame)))
		{
			wereThereErrors = true;
			goto OnExit;
		}
		s_FrameBuffer.Bind();

		if (!s_DrawBuffer.Initialize(eae6320::ConstantBuffer::sBufferType::drawCallType, sizeof(eae6320::ConstantBuffer::sDrawCall), reinterpret_cast<void*>(&s_Draw)))
		{
			wereThereErrors = true;
			goto OnExit;
		}
		s_DrawBuffer.Bind();

	OnExit:

		return !wereThereErrors;
	}

	bool CreateProgram()
	{
		return true;
	}

	bool CreateRenderingContext()
	{
		{
			s_deviceContext = GetDC( s_renderingWindow );
			if ( s_deviceContext == NULL )
			{
				EAE6320_ASSERT( false );
				eae6320::Logging::OutputError( "Windows failed to get the device context" );
				return false;
			}
		}
		{
			int pixelFormatId;
			{
				const int desiredAttributes[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, 24,
					WGL_RED_BITS_ARB, 8,
					WGL_GREEN_BITS_ARB, 8,
					WGL_BLUE_BITS_ARB, 8,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 8,
					NULL
				};
				const float* const noFloatAttributes = NULL;
				const unsigned int onlyReturnBestMatch = 1;
				unsigned int returnedFormatCount;
				if ( wglChoosePixelFormatARB( s_deviceContext, desiredAttributes, noFloatAttributes, onlyReturnBestMatch,
					&pixelFormatId, &returnedFormatCount ) != FALSE )
				{
					if ( returnedFormatCount == 0 )
					{
						EAE6320_ASSERT( false );
						eae6320::Logging::OutputError( "Windows couldn't find a pixel format that satisfied the desired attributes" );
						return false;
					}
				}
				else
				{
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
					EAE6320_ASSERTF( false, windowsErrorMessage.c_str() );
					eae6320::Logging::OutputError( "Windows failed to choose the closest pixel format: %s", windowsErrorMessage.c_str() );
					return false;
				}
			}
			{
				PIXELFORMATDESCRIPTOR pixelFormatDescriptor = { 0 };
				{
					pixelFormatDescriptor.nSize = sizeof( PIXELFORMATDESCRIPTOR );
					pixelFormatDescriptor.nVersion = 1;
					pixelFormatDescriptor.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
					pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
					pixelFormatDescriptor.cColorBits = 24;
					pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
					pixelFormatDescriptor.cDepthBits = 24;
					pixelFormatDescriptor.cStencilBits = 8;
				}
				if ( SetPixelFormat( s_deviceContext, pixelFormatId, &pixelFormatDescriptor ) == FALSE )
				{
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
					EAE6320_ASSERTF( false, windowsErrorMessage.c_str() );
					eae6320::Logging::OutputError( "Windows couldn't set the desired pixel format: %s", windowsErrorMessage.c_str() );
					return false;
				}
			}
		}
		{
			{
				const int desiredAttributes[] =
				{
					WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
					WGL_CONTEXT_MINOR_VERSION_ARB, 2,
					WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
					WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
					NULL
				};
				const HGLRC noSharedContexts = NULL;
				s_openGlRenderingContext = wglCreateContextAttribsARB( s_deviceContext, noSharedContexts, desiredAttributes );
				if ( s_openGlRenderingContext == NULL )
				{
					DWORD errorCode;
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError( &errorCode );
					std::ostringstream errorMessage;
					errorMessage << "Windows failed to create an OpenGL rendering context: ";
					if ( ( errorCode == ERROR_INVALID_VERSION_ARB )
						|| ( HRESULT_CODE( errorCode ) == ERROR_INVALID_VERSION_ARB ) )
					{
						errorMessage << "The requested version number is invalid";
					}
					else if ( ( errorCode == ERROR_INVALID_PROFILE_ARB )
						|| ( HRESULT_CODE( errorCode ) == ERROR_INVALID_PROFILE_ARB ) )
					{
						errorMessage << "The requested profile is invalid";
					}
					else
					{
						errorMessage << windowsErrorMessage;
					}
					EAE6320_ASSERTF( false, errorMessage.str().c_str() );
					eae6320::Logging::OutputError( errorMessage.str().c_str() );
						
					return false;
				}
			}
			if ( wglMakeCurrent( s_deviceContext, s_openGlRenderingContext ) == FALSE )
			{
				const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
				EAE6320_ASSERTF( false, windowsErrorMessage.c_str() );
				eae6320::Logging::OutputError( "Windows failed to set the current OpenGL rendering context: %s",
					windowsErrorMessage.c_str() );
				return false;
			}
		}

		return true;
	}

	bool EnableBackfaceCulling()
	{
		bool wereThereErrors = false;
		
		glEnable(GL_CULL_FACE);

		GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to enable backface culling: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}

		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);

		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed depth test: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		glDepthMask(GL_TRUE);
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed depth writing: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}

		return !wereThereErrors;
	}

	bool CreateSampler()
	{
		{
			const GLsizei samplerStateCount = 1;
			glGenSamplers(samplerStateCount, &s_samplerState);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				if (s_samplerState != 0)
				{
					glSamplerParameteri(s_samplerState, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
					glSamplerParameteri(s_samplerState, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
					glSamplerParameteri(s_samplerState, GL_TEXTURE_WRAP_S, GL_REPEAT);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
					glSamplerParameteri(s_samplerState, GL_TEXTURE_WRAP_T, GL_REPEAT);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
				}
				else
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("OpenGL failed to create a sampler state");
					return false;
				}
			}
			else
			{
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to create a sampler state: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return false;
			}
		}
		{
			const GLuint maxTextureUnitCountYouThinkYoullUse = 5;
			for (GLuint i = 0; i < maxTextureUnitCountYouThinkYoullUse; ++i)
			{
				glBindSampler(i, s_samplerState);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to bind the sampler state to texture unit %u: %s",
						i, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return false;
				}
			}
		}
		return true;
	}
}