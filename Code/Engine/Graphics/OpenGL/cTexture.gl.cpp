#include "../cTexture.h"
#include <algorithm>
#include "../cTexture/Internal.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"

void eae6320::Graphics::cTexture::Bind( const unsigned int i_id ) const
{
	{
		glActiveTexture( GL_TEXTURE0 + static_cast<GLint>( i_id ) );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
	{
		EAE6320_ASSERT( m_textureId != 0 );
		glBindTexture( GL_TEXTURE_2D, m_textureId );
		EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
	}
}

bool eae6320::Graphics::cTexture::CleanUp()
{
	bool wereThereErrors = false;

	if ( m_textureId != 0 )
	{
		const GLsizei textureCount = 1;
		glDeleteTextures( textureCount, &m_textureId );
		const GLenum errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			wereThereErrors = true;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the texture: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		m_textureId = 0;
	}

	return !wereThereErrors;
}

bool eae6320::Graphics::cTexture::Initialize( const char* const i_path, const sDdsInfo& i_ddsInfo )
{
	bool wereThereErrors = false;
	{
		const GLsizei textureCount = 1;
		glGenTextures( textureCount, &m_textureId );
		const GLenum errorCode = glGetError();
		if ( errorCode == GL_NO_ERROR )
		{
			if ( m_textureId != 0 )
			{
				glBindTexture( GL_TEXTURE_2D, m_textureId );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to bind the new texture for %s: %s",
						i_path, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					goto OnExit;
				}
			}
			else
			{
				bool wereThereErrors = true;
				EAE6320_ASSERT( false );
				eae6320::Logging::OutputError( "OpenGL failed to create a texture for %s", i_path );
				goto OnExit;
			}
		}
		else
		{
			bool wereThereErrors = true;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to create a texture for %s: %s",
				i_path, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			goto OnExit;
		}
	}
	{
		GLsizei currentWidth = static_cast<GLsizei>( i_ddsInfo.width );
		GLsizei currentHeight = static_cast<GLsizei>( i_ddsInfo.height );
		const uint8_t* currentPosition = reinterpret_cast<const uint8_t*>( i_ddsInfo.imageData );
		const uint8_t* const endOfFile = currentPosition + i_ddsInfo.imageDataSize;
		const GLsizei blockSize = static_cast<GLsizei>( TextureFormats::GetSizeOfBlock( i_ddsInfo.format ) );
		const GLint borderWidth = 0;
		GLenum glFormat = 0;
		switch ( i_ddsInfo.format )
		{
			case TextureFormats::BC1: glFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT; break;
			case TextureFormats::BC3: glFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
		}
		for ( GLint i = 0; i < static_cast<GLint>( i_ddsInfo.mipLevelCount ); ++i )
		{
			const GLsizei blockCount_singleRow = ( currentWidth + 3 ) / 4;
			const GLsizei byteCount_singleRow = blockCount_singleRow * blockSize;
			const GLsizei rowCount = ( currentHeight + 3 ) / 4;
			const GLsizei byteCount_currentMipLevel = byteCount_singleRow * rowCount;
			glCompressedTexImage2D( GL_TEXTURE_2D, i, glFormat, currentWidth, currentHeight,
				borderWidth, byteCount_currentMipLevel, currentPosition );
			const GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				{
					currentPosition += byteCount_currentMipLevel;
					if ( currentPosition <= endOfFile )
					{
						currentWidth = std::max( currentWidth / 2, 1 );
						currentHeight = std::max( currentHeight / 2, 1 );
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERTF( false, "DDS file not big enough" );
						Logging::OutputError( "The DDS file %s isn't big enough to hold the data that its header claims to", i_path );
						goto OnExit;
					}
				
				}
			}
			else
			{
				bool wereThereErrors = true;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to copy the texture data from MIP level %u of %s: %s",
					i, i_path, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				goto OnExit;
			}
		}
		EAE6320_ASSERTF( currentPosition == endOfFile, "The DDS file %s has more data than it should", i_path );
	}

OnExit:

	if ( wereThereErrors && ( m_textureId != 0 ) )
	{
		const GLsizei textureCount = 1;
		glDeleteTextures( textureCount, &m_textureId );
		EAE6320_ASSERT( glGetError == GL_NO_ERROR );
		m_textureId = 0;
	}
	
	return !wereThereErrors;
}
