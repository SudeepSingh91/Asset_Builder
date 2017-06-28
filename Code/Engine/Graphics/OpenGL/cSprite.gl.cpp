#include "../cSprite.h"
#include "../VertexFormat.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"

GLuint eae6320::Graphics::cSprite::ms_vertexArrayId = 0;
GLuint eae6320::Graphics::cSprite::ms_vertexBufferId = 0;

namespace
{
	const unsigned int s_vertexCount = 4;
}

void eae6320::Graphics::cSprite::Draw() const
{
	{
		VertexFormat::sVertex vertexData[s_vertexCount];
		{
			{
				{
					VertexFormat::sVertex& vertex = vertexData[0];
					vertex.x = m_position_screen.left;
					vertex.y = m_position_screen.bottom;
					vertex.z = -1.0f;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[1];
					vertex.x = m_position_screen.right;
					vertex.y = m_position_screen.bottom;
					vertex.z = -1.0f;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[2];
					vertex.x = m_position_screen.left;
					vertex.y = m_position_screen.top;
					vertex.z = -1.0f;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[3];
					vertex.x = m_position_screen.right;
					vertex.y = m_position_screen.top;
					vertex.z = -1.0f;
				}
			}
			{
				{
					VertexFormat::sVertex& vertex = vertexData[0];
					vertex.u = m_textureCoordinates.left;
					vertex.v = m_textureCoordinates.bottom;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[1];
					vertex.u = m_textureCoordinates.right;
					vertex.v = m_textureCoordinates.bottom;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[2];
					vertex.u = m_textureCoordinates.left;
					vertex.v = m_textureCoordinates.top;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[3];
					vertex.u = m_textureCoordinates.right;
					vertex.v = m_textureCoordinates.top;
				}
			}
			for ( unsigned int i = 0; i < s_vertexCount; ++i )
			{
				VertexFormat::sVertex& vertex = vertexData[i];
				vertex.r = vertex.g = vertex.b = vertex.a = 255;
			}
		}
		{
			glBindBuffer( GL_ARRAY_BUFFER, ms_vertexBufferId );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
		{
			glInvalidateBufferData( ms_vertexBufferId );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
		{
			glBufferData( GL_ARRAY_BUFFER, static_cast<GLsizeiptr>( sizeof( vertexData ) ), vertexData, GL_STREAM_DRAW );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
	}
	{
		glBindVertexArray( ms_vertexArrayId );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
	{
		const GLenum mode = GL_TRIANGLE_STRIP;
		const GLint indexOfFirstVertexToRender = 0;
		glDrawArrays( mode, indexOfFirstVertexToRender, static_cast<GLsizei>( s_vertexCount ) );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
}

bool eae6320::Graphics::cSprite::Initialize()
{
	bool wereThereErrors = false;
	{
		{
			const GLsizei arrayCount = 1;
			glGenVertexArrays( arrayCount, &ms_vertexArrayId );
			const GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glBindVertexArray( ms_vertexArrayId );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to bind the sprites' new vertex array: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to get an unused vertex array ID for sprites: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		{
			const GLsizei bufferCount = 1;
			glGenBuffers( bufferCount, &ms_vertexBufferId );
			const GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glBindBuffer( GL_ARRAY_BUFFER, ms_vertexBufferId );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to bind the sprites' new vertex buffer: %s",
						reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to get an unused vertex buffer ID for the sprites: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		{
			VertexFormat::sVertex vertexData[s_vertexCount];
			{
				{
					VertexFormat::sVertex& vertex = vertexData[0];
					vertex.x = -1.0f;
					vertex.y = -1.0f;
					vertex.z = -1.0f;
					vertex.u = 0.0f;
					vertex.v = 0.0f;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[1];
					vertex.x = 1.0f;
					vertex.y = -1.0f;
					vertex.z = -1.0f;
					vertex.u = 1.0f;
					vertex.v = 0.0f;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[2];
					vertex.x = -1.0f;
					vertex.y = 1.0f;
					vertex.z = -1.0f;
					vertex.u = 0.0f;
					vertex.v = 1.0f;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[3];
					vertex.x = 1.0f;
					vertex.y = 1.0f;
					vertex.z = -1.0f;
					vertex.u = 1.0f;
					vertex.v = 1.0f;
					vertex.r = vertex.g = vertex.b = vertex.a = 255;
				}
			}
			glBufferData( GL_ARRAY_BUFFER, sizeof( vertexData ), reinterpret_cast<const GLvoid*>( vertexData ),
				GL_STREAM_DRAW );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				wereThereErrors = true;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to allocate the sprites' vertex buffer: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		{
			const GLsizei stride = sizeof( eae6320::Graphics::VertexFormat::sVertex );
			{
				const GLuint vertexElementLocation = 0;
				const GLint elementCount = 3;
				const GLboolean notNormalized = GL_FALSE;	
				glVertexAttribPointer( vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
					reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormat::sVertex, x ) ) );
				const GLenum errorCode = glGetError();
				if ( errorCode == GL_NO_ERROR )
				{
					glEnableVertexAttribArray( vertexElementLocation );
					const GLenum errorCode = glGetError();
					if ( errorCode != GL_NO_ERROR )
					{
						wereThereErrors = true;
						EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
						eae6320::Logging::OutputError( "OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
							vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to set the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			{
				const GLuint vertexElementLocation = 1;
				const GLint elementCount = 4;
				const GLboolean isNormalized = GL_TRUE;
				glVertexAttribPointer( vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, isNormalized, stride,
					reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormat::sVertex, r ) ) );
				const GLenum errorCode = glGetError();
				if ( errorCode == GL_NO_ERROR )
				{
					glEnableVertexAttribArray( vertexElementLocation );
					const GLenum errorCode = glGetError();
					if ( errorCode != GL_NO_ERROR )
					{
						wereThereErrors = true;
						EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
						eae6320::Logging::OutputError( "OpenGL failed to enable the COLOR0 vertex attribute at location %u: %s",
							vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to set the COLOR0 vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			{
				const GLuint vertexElementLocation = 2;
				const GLint elementCount = 2;
				const GLboolean notNormalized = GL_FALSE;	
				glVertexAttribPointer( vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
					reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormat::sVertex, u ) ) );
				const GLenum errorCode = glGetError();
				if ( errorCode == GL_NO_ERROR )
				{
					glEnableVertexAttribArray( vertexElementLocation );
					const GLenum errorCode = glGetError();
					if ( errorCode != GL_NO_ERROR )
					{
						wereThereErrors = true;
						EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
						eae6320::Logging::OutputError( "OpenGL failed to enable the TEXCOORD0 vertex attribute at location %u: %s",
							vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to set the TEXCOORD0 vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
		}
	}

OnExit:

	return !wereThereErrors;
}

bool eae6320::Graphics::cSprite::CleanUp()
{
	bool wereThereErrors = false;
	{
		glBindVertexArray( 0 );
		const GLenum errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			wereThereErrors = true;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to unbind all vertex arrays before deleting cleaning up the sprites: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
	}
	if ( ms_vertexBufferId != 0 )
	{
		const GLsizei bufferCount = 1;
		glDeleteBuffers( bufferCount, &ms_vertexBufferId );
		const GLenum errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			wereThereErrors = true;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the sprites' vertex buffer: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		ms_vertexBufferId = 0;
	}
	if ( ms_vertexArrayId != 0 )
	{
		const GLsizei arrayCount = 1;
		glDeleteVertexArrays( arrayCount, &ms_vertexArrayId );
		const GLenum errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the sprites' vertex array: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		ms_vertexArrayId = 0;
	}

	return !wereThereErrors;
}
