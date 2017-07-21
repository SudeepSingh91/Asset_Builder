#define NOMINMAX

#include "../cTexture.h"
#include "../Context.h"

#include <algorithm>
#include <cstdlib>
#include "../../Windows/Includes.h"
#include <D3D11.h>
#include "../cTexture/Internal.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"

namespace
{
	Context* myCont = Context::GetContext();
}

void Engine::Graphics::cTexture::Bind( const unsigned int i_id ) const
{
	const unsigned int viewCount = 1;
	myCont->s_direct3dImmediateContext->PSSetShaderResources( i_id, viewCount, &m_textureView );
}

bool Engine::Graphics::cTexture::CleanUp()
{
	if ( m_textureView )
	{
		m_textureView->Release();
		m_textureView = NULL;
	}

	return true;
}

bool Engine::Graphics::cTexture::Initialize( const char* const i_path, const sDdsInfo& i_ddsInfo )
{
	bool wereThereErrors = false;

	ID3D11Texture2D* resource = NULL;
	D3D11_SUBRESOURCE_DATA* subResourceData = NULL;

	{
		const size_t byteCountToAllocate = sizeof(D3D11_SUBRESOURCE_DATA) * i_ddsInfo.mipLevelCount;
		subResourceData = reinterpret_cast<D3D11_SUBRESOURCE_DATA*>( malloc( byteCountToAllocate ) );
		if ( !subResourceData )
		{
			wereThereErrors = true;
			ASSERTF( false, "Failed to allocate %u bytes", byteCountToAllocate );
			Logging::OutputError( "Failed to allocate %u bytes for %u subresources for %s", byteCountToAllocate, i_ddsInfo.mipLevelCount, i_path );
			goto OnExit;
		}
	}
	{
		unsigned int currentWidth = static_cast<unsigned int>( i_ddsInfo.width );
		unsigned int currentHeight = static_cast<unsigned int>( i_ddsInfo.height );
		const uint8_t* currentPosition = reinterpret_cast<const uint8_t*>( i_ddsInfo.imageData );
		const uint8_t* const endOfFile = currentPosition + i_ddsInfo.imageDataSize;
		const unsigned int blockSize = TextureFormats::GetSizeOfBlock( i_ddsInfo.format );
		for ( unsigned int i = 0; i < i_ddsInfo.mipLevelCount; ++i )
		{
			const unsigned int blockCount_singleRow = ( currentWidth + 3 ) / 4;
			const unsigned int byteCount_singleRow = blockCount_singleRow * blockSize;
			const unsigned int rowCount = ( currentHeight + 3 ) / 4;
			const unsigned int byteCount_currentMipLevel = byteCount_singleRow * rowCount;
			{
				D3D11_SUBRESOURCE_DATA& currentSubResourceData = subResourceData[i];
				currentSubResourceData.pSysMem = currentPosition;
				currentSubResourceData.SysMemPitch = byteCount_singleRow;
				currentSubResourceData.SysMemSlicePitch = byteCount_currentMipLevel;
			}
			{
				currentPosition += byteCount_currentMipLevel;
				if ( currentPosition <= endOfFile )
				{
					currentWidth = std::max( currentWidth / 2, 1u );
					currentHeight = std::max( currentHeight / 2, 1u );
				}
				else
				{
					wereThereErrors = true;
					ASSERTF( false, "DDS file not big enough" );
					Logging::OutputError( "The DDS file %s isn't big enough to hold the data that its header claims to", i_path );
					goto OnExit;
				}
			}
		}
		ASSERTF( currentPosition == endOfFile, "The DDS file %s has more data than it should", i_path );
	}
	DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
	switch ( i_ddsInfo.format )
	{
		case TextureFormats::BC1: dxgiFormat = DXGI_FORMAT_BC1_UNORM; break;
		case TextureFormats::BC3: dxgiFormat = DXGI_FORMAT_BC3_UNORM; break;
	}
	{
		D3D11_TEXTURE2D_DESC textureDescription = { 0 };
		{
			textureDescription.Width = i_ddsInfo.width;
			textureDescription.Height = i_ddsInfo.height;
			textureDescription.MipLevels = i_ddsInfo.mipLevelCount;
			textureDescription.ArraySize = 1;
			textureDescription.Format = dxgiFormat;
			{
				DXGI_SAMPLE_DESC& sampleDescription = textureDescription.SampleDesc;
				sampleDescription.Count = 1;	
				sampleDescription.Quality = 0;	
			}
			textureDescription.Usage = D3D11_USAGE_IMMUTABLE;	
			textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDescription.CPUAccessFlags = 0;	
			textureDescription.MiscFlags = 0;
		}
		const HRESULT result = myCont->s_direct3dDevice->CreateTexture2D( &textureDescription, subResourceData, &resource );
		if ( FAILED( result ) )
		{
			wereThereErrors = true;
			ASSERTF( false, "CreateTexture2D() failed" );
			Engine::Logging::OutputError( "Direct3D failed to create a texture from %s with HRESULT %#010x", i_path, result );
			goto OnExit;
		}
	}
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription;
		{
			shaderResourceViewDescription.Format = dxgiFormat;
			shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			{
				D3D11_TEX2D_SRV& shaderResourceView2dDescription = shaderResourceViewDescription.Texture2D;
				shaderResourceView2dDescription.MostDetailedMip = 0;	
				shaderResourceView2dDescription.MipLevels = -1;	
			}
		}
		const HRESULT result = myCont->s_direct3dDevice->CreateShaderResourceView( resource, &shaderResourceViewDescription, &m_textureView );
		if ( FAILED( result ) )
		{
			wereThereErrors = true;
			ASSERTF( false, "CreateShaderResourceView() failed" );
			Engine::Logging::OutputError( "Direct3D failed to create a shader resource view for %s with HRESULT %#010x", i_path, result );
			goto OnExit;
		}
	}

OnExit:
	if ( resource )
	{
		resource->Release();
		resource = NULL;
	}
	if ( subResourceData )
	{
		free( subResourceData );
		subResourceData = NULL;
	}

	return !wereThereErrors;
}
