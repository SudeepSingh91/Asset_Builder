#include "../cTexture.h"
#include "Internal.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../Platform/Platform.h"
#include "../../../External/DirectXTex/DirectXTex/DDS.h"

bool Engine::Graphics::cTexture::Load( const char* const i_path )
{
	bool wereThereErrors = false;

	Platform::sDataFromFile dataFromFile;

	{
		std::string errorMessage;
		if ( !Platform::LoadBinaryFile( i_path, dataFromFile, &errorMessage ) )
		{
			wereThereErrors = true;
			ASSERTF( false, errorMessage.c_str() );
			Logging::OutputError( "Failed to load texture data from file %s: %s", i_path, errorMessage.c_str() );
			goto OnExit;
		}
	}

	const uint8_t* currentPosition = reinterpret_cast<uint8_t*>( dataFromFile.data );
	size_t remainingSize = dataFromFile.size;
	if ( ( remainingSize >= sizeof( DirectX::DDS_MAGIC ) )
		&& ( *reinterpret_cast<const uint32_t*>( currentPosition ) == DirectX::DDS_MAGIC ) )
	{
		currentPosition += sizeof( DirectX::DDS_MAGIC );
		remainingSize -= sizeof( DirectX::DDS_MAGIC );
	}
	else
	{
		wereThereErrors = true;
		ASSERTF( false, "Invalid DDS file %s", i_path );
		Logging::OutputError( "The DDS file %s doesn't start with the magic number", i_path );
		goto OnExit;
	}
	const DirectX::DDS_HEADER* header;
	if ( remainingSize >= sizeof( DirectX::DDS_HEADER ) )
	{
		header = reinterpret_cast<const DirectX::DDS_HEADER*>( currentPosition );
		currentPosition += sizeof( DirectX::DDS_HEADER );
		remainingSize -= sizeof( DirectX::DDS_HEADER );
		if ( ( header->dwSize != sizeof( DirectX::DDS_HEADER ) )
			|| ( header->ddspf.dwSize != sizeof( DirectX::DDS_PIXELFORMAT ) ) )
		{
			wereThereErrors = true;
			ASSERTF( false, "Invalid DDS file %s", i_path );
			Logging::OutputError( "The header in the DDS file %s has the wrong struct sizes", i_path );
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		ASSERTF( false, "Invalid DDS file %s", i_path );
		Logging::OutputError( "The DDS file %s isn't big enough for a header", i_path );
		goto OnExit;
	}
	const DirectX::DDS_HEADER_DXT10* header_dxt10 = NULL;
	if ( ( header->ddspf.dwFlags & DDS_FOURCC )
		&& ( header->ddspf.dwFourCC == MAKEFOURCC( 'D', 'X', '1', '0' ) ) )
	{
		if ( remainingSize >= sizeof( DirectX::DDS_HEADER_DXT10 ) )
		{
			header_dxt10 = reinterpret_cast<const DirectX::DDS_HEADER_DXT10*>( currentPosition );
			currentPosition += sizeof( DirectX::DDS_HEADER_DXT10 );
			remainingSize -= sizeof( DirectX::DDS_HEADER_DXT10 );
		}
		else
		{
			wereThereErrors = true;
			ASSERTF( false, "Invalid DDS file %s", i_path );
			Logging::OutputError( "The DDS file %s isn't big enough for a DXT10 header", i_path );
			goto OnExit;
		}
	}

	{
		if ( header_dxt10 )
		{
			wereThereErrors = true;
			ASSERT( false );
			Logging::OutputError( "The DDS file %s uses a DXT10 header, which is unsupported", i_path );
			goto OnExit;
		}
		if ( header->dwFlags & DDS_HEADER_FLAGS_VOLUME )
		{
			wereThereErrors = true;
			ASSERT( false );
			Logging::OutputError( "The DDS file %s is a volume texture, which is unsupported", i_path );
			goto OnExit;
		}
		else if ( header->dwCaps & DDS_CUBEMAP )
		{
			wereThereErrors = true;
			ASSERT( false );
			Logging::OutputError( "The DDS file %s is a cube texture, which is unsupported", i_path );
			goto OnExit;
		}
		if ( header->ddspf.dwFlags & DDS_FOURCC )
		{
			if ( ( header->ddspf.dwFourCC != DirectX::DDSPF_DXT1.dwFourCC )
				&& ( header->ddspf.dwFourCC != DirectX::DDSPF_DXT5.dwFourCC ) )
			{
				wereThereErrors = true;
				ASSERT( false );
				Logging::OutputError( "The DDS file %s isn't one of the supported formats: BC1, BC3", i_path );
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			ASSERT( false );
			Logging::OutputError( "The DDS file %s doesn't use a FourCC pixel format, which is unsupported", i_path );
			goto OnExit;
		}
	}

	{
		sDdsInfo ddsInfo;
		{
			{
				ddsInfo.imageData = currentPosition;
				ddsInfo.imageDataSize = remainingSize;
			}
			ddsInfo.width = static_cast<unsigned int>( header->dwWidth );
			ddsInfo.height = static_cast<unsigned int>( header->dwHeight );
			ddsInfo.mipLevelCount = static_cast<unsigned int>( header->dwMipMapCount );
			{
				if ( header->ddspf.dwFourCC == DirectX::DDSPF_DXT1.dwFourCC )
				{
					ddsInfo.format = TextureFormats::BC1;
				}
				else if ( header->ddspf.dwFourCC == DirectX::DDSPF_DXT5.dwFourCC )
				{
					ddsInfo.format = TextureFormats::BC3;
				}
				else
				{
					ddsInfo.format = TextureFormats::Unknown;
				}
			}
		}
		if ( !Initialize( i_path, ddsInfo ) )
		{
			goto OnExit;
		}
	}

OnExit:

	dataFromFile.Free();

	return !wereThereErrors;
}

Engine::Graphics::cTexture::cTexture()
	:
#if defined( PLATFORM_D3D )
	m_textureView( NULL )
#elif defined( PLATFORM_GL )
	m_textureId( 0 )
#endif
{

}

Engine::Graphics::cTexture::~cTexture()
{
	CleanUp();
}
