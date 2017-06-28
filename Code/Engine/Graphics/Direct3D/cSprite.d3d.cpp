#include "../cSprite.h"
#include "../../Windows/Includes.h"
#include <D3D11.h>
#include "../VertexFormat.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../Context.h"

ID3D11Buffer* eae6320::Graphics::cSprite::ms_vertexBuffer = NULL;

namespace
{
	const unsigned int s_vertexCount = 4;
	Context* myCont = Context::GetContext();
}

void eae6320::Graphics::cSprite::Draw() const
{
	EAE6320_ASSERT( ms_vertexBuffer != NULL );
	{
		void* memoryToWriteTo;
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubResource;
			{
				const unsigned int noSubResources = 0;
				const D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
				const unsigned int noFlags = 0;
				const HRESULT result = myCont->s_direct3dImmediateContext->Map( ms_vertexBuffer, noSubResources, mapType, noFlags, &mappedSubResource );
				if ( FAILED( result ) )
				{
					EAE6320_ASSERT( false );
					Logging::OutputError( "Direct3D failed to map the sprite vertex buffer" );
					return;
				}
			}
			memoryToWriteTo = mappedSubResource.pData;
		}
		{
			VertexFormat::sVertex* const vertexData = reinterpret_cast<VertexFormat::sVertex*>( memoryToWriteTo );
			{
				{
					VertexFormat::sVertex& vertex = vertexData[0];
					vertex.x = m_position_screen.left;
					vertex.y = m_position_screen.bottom;
					vertex.z = 0.0f;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[1];
					vertex.x = m_position_screen.left;
					vertex.y = m_position_screen.top;
					vertex.z = 0.0f;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[2];
					vertex.x = m_position_screen.right;
					vertex.y = m_position_screen.bottom;
					vertex.z = 0.0f;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[3];
					vertex.x = m_position_screen.right;
					vertex.y = m_position_screen.top;
					vertex.z = 0.0f;
				}
			}
			{
				{
					VertexFormat::sVertex& vertex = vertexData[0];
					vertex.u = m_textureCoordinates.left;
					vertex.v = 1.0f - m_textureCoordinates.bottom;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[1];
					vertex.u = m_textureCoordinates.left;
					vertex.v = 1.0f - m_textureCoordinates.top;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[2];
					vertex.u = m_textureCoordinates.right;
					vertex.v = 1.0f - m_textureCoordinates.bottom;
				}
				{
					VertexFormat::sVertex& vertex = vertexData[3];
					vertex.u = m_textureCoordinates.right;
					vertex.v = 1.0f - m_textureCoordinates.top;
				}
			}
			for ( unsigned int i = 0; i < s_vertexCount; ++i )
			{
				VertexFormat::sVertex& vertex = vertexData[i];
				vertex.r = vertex.g = vertex.b = vertex.a = 255;
			}
		}
		{
			const unsigned int noSubResources = 0;
			myCont->s_direct3dImmediateContext->Unmap( ms_vertexBuffer, noSubResources );
		}
	}
	{
		const unsigned int startingSlot = 0;
		const unsigned int vertexBufferCount = 1;
		const unsigned int bufferStride = sizeof( VertexFormat::sVertex );
		const unsigned int bufferOffset = 0;
		myCont->s_direct3dImmediateContext->IASetVertexBuffers( startingSlot, vertexBufferCount, &ms_vertexBuffer, &bufferStride, &bufferOffset );
	}
	{
		myCont->s_direct3dImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	}
	{
		const unsigned int indexOfFirstVertexToUse = 0;
		myCont->s_direct3dImmediateContext->Draw( s_vertexCount, indexOfFirstVertexToUse );
	}
}

bool eae6320::Graphics::cSprite::Initialize()
{
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	{
		const size_t bufferSize = static_cast<size_t>( s_vertexCount ) * sizeof( VertexFormat::sVertex );
		EAE6320_ASSERT( bufferSize < ( uint64_t( 1u ) << ( sizeof( bufferDescription.ByteWidth ) * 8 ) ) );
		bufferDescription.ByteWidth = static_cast<unsigned int>( bufferSize );
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;	
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;
	}
	VertexFormat::sVertex vertexData[s_vertexCount];
	{
		{
			VertexFormat::sVertex& vertex = vertexData[0];
			vertex.x = -1.0f;
			vertex.y = -1.0f;
			vertex.z = 0.0f;
			vertex.u = 0.0f;
			vertex.v = 1.0f;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
		{
			VertexFormat::sVertex& vertex = vertexData[1];
			vertex.x = -1.0f;
			vertex.y = 1.0f;
			vertex.z = 0.0f;
			vertex.u = 0.0f;
			vertex.v = 0.0f;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
		{
			VertexFormat::sVertex& vertex = vertexData[2];
			vertex.x = 1.0f;
			vertex.y = -1.0f;
			vertex.z = 0.0f;
			vertex.u = 1.0f;
			vertex.v = 1.0f;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
		{
			VertexFormat::sVertex& vertex = vertexData[3];
			vertex.x = 1.0f;
			vertex.y = 1.0f;
			vertex.z = 0.0f;
			vertex.u = 1.0f;
			vertex.v = 0.0f;
			vertex.r = vertex.g = vertex.b = vertex.a = 255;
		}
	}
	D3D11_SUBRESOURCE_DATA initialData = { 0 };
	{
		initialData.pSysMem = vertexData;
	}
	const HRESULT result = myCont->s_direct3dDevice->CreateBuffer( &bufferDescription, &initialData, &ms_vertexBuffer );
	if ( SUCCEEDED( result ) )
	{
		return true;
	}
	else
	{
		EAE6320_ASSERT( false );
		eae6320::Logging::OutputError( "Direct3D failed to create the vertex buffer for sprites with HRESULT %#010x", result );
		return false;
	}
}

bool eae6320::Graphics::cSprite::CleanUp()
{
	if ( ms_vertexBuffer )
	{
		ms_vertexBuffer->Release();
		ms_vertexBuffer = NULL;
	}

	return true;
}
