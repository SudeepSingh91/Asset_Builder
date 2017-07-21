#include "../Mesh.h"
#include "../Context.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"

namespace Engine
{
	namespace Mesh
	{	
		bool Mesh::Initialize()
		{
			Context* myCont = Context::GetContext();
			{
			}

			unsigned int bufferSize = s_numVertices * sizeof(sVertex);

			D3D11_BUFFER_DESC bufferDescription = { 0 };
			{
				bufferDescription.ByteWidth = bufferSize;
				bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	
				bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDescription.CPUAccessFlags = 0;	
				bufferDescription.MiscFlags = 0;
				bufferDescription.StructureByteStride = 0;	
			}
			D3D11_SUBRESOURCE_DATA initialData = { 0 };
			{
				initialData.pSysMem = reinterpret_cast<void *>(vertexArray);
			}

			const HRESULT result = myCont->s_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &s_vertexBuffer);
			if (FAILED(result))
			{
				ASSERT(false);
				Engine::Logging::OutputError("Direct3D failed to create the vertex buffer with HRESULT %#010x", result);
				return false;
			}

			unsigned int bufferSizeInd = s_numIndices * sizeof(uint16_t);

			D3D11_BUFFER_DESC bufferDescriptionInd = { 0 };
			{
				bufferDescriptionInd.ByteWidth = bufferSizeInd;
				bufferDescriptionInd.Usage = D3D11_USAGE_IMMUTABLE;	
				bufferDescriptionInd.BindFlags = D3D11_BIND_INDEX_BUFFER;
				bufferDescriptionInd.CPUAccessFlags = 0;	
				bufferDescriptionInd.MiscFlags = 0;
				bufferDescriptionInd.StructureByteStride = 0;	
			}
			D3D11_SUBRESOURCE_DATA initialDataInd = { 0 };
			{
				initialDataInd.pSysMem = reinterpret_cast<void*>(indiceArray);
			}

			const HRESULT resultInd = myCont->s_direct3dDevice->CreateBuffer(&bufferDescriptionInd, &initialDataInd, &s_indexBuffer);
			if (FAILED(resultInd))
			{
				ASSERT(false);
				Engine::Logging::OutputError("Direct3D failed to create the index buffer with HRESULT %#010x", result);
				return false;
			}

			return true;
		}

		bool Mesh::CleanUp()
		{
			bool wereThereErrors = false;
			
			if (s_vertexBuffer)
			{
				s_vertexBuffer->Release();
				s_vertexBuffer = NULL;
			}
			if (s_indexBuffer)
			{
				s_indexBuffer->Release();
				s_indexBuffer = NULL;
			}
			return !wereThereErrors;
		}

		void Mesh::Draw()
		{
			Context* myCont = Context::GetContext();
			{
				const unsigned int startingSlot = 0;
				const unsigned int vertexBufferCount = 1;
				const unsigned int bufferStride = sizeof(sVertex);
				const unsigned int bufferOffset = 0;
				myCont->s_direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &s_vertexBuffer, &bufferStride, &bufferOffset);
			}
			{
				ASSERT(s_indexBuffer != NULL);
				const DXGI_FORMAT format = DXGI_FORMAT_R16_UINT;
				const unsigned int offset = 0;
				myCont->s_direct3dImmediateContext->IASetIndexBuffer(s_indexBuffer, format, offset);
			}
			{
				const unsigned int indexOfFirstIndexToUse = 0;
				const unsigned int offsetToAddToEachIndex = 0;
				myCont->s_direct3dImmediateContext->DrawIndexed(s_numIndices, indexOfFirstIndexToUse, offsetToAddToEachIndex);
			}
		}
	}
}