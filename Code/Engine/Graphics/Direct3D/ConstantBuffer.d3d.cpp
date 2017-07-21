#include "../ConstantBuffer.h"
#include "../../Math/Functions.h"

namespace Engine
{
	namespace ConstantBuffer
	{
		Context* myCont = Context::GetContext();
		
		bool ConstantBuffer::CreateBuffer(void* i_initialData)
		{
			D3D11_BUFFER_DESC bufferDescription = { 0 };
			{
				bufferDescription.ByteWidth = Math::RoundUpToMultiple_powerOf2(static_cast<unsigned int>(s_BufferSize), 16u);
				bufferDescription.Usage = D3D11_USAGE_DYNAMIC;	
				bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	
				bufferDescription.MiscFlags = 0;
				bufferDescription.StructureByteStride = 0;	
			}

			D3D11_SUBRESOURCE_DATA initialData = { 0 };
			if (s_type == 0)
			{
				initialData.pSysMem = reinterpret_cast<sFrame*>(i_initialData);
			}
			else if (s_type == 1)
			{
				initialData.pSysMem = reinterpret_cast<sDrawCall*>(i_initialData);
			}
			else if (s_type == 2)
			{
				initialData.pSysMem = reinterpret_cast<sMaterial*>(i_initialData);
			}

			const HRESULT result = myCont->s_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &s_constantBuffer);
			if (SUCCEEDED(result))
			{
				return true;
			}
			else
			{
				ASSERT(false);
				Engine::Logging::OutputError("Direct3D failed to create a constant buffer with HRESULT %#010x", result);
				return false;
			}
		}

		void ConstantBuffer::Bind()
		{
			const unsigned int bufferCount = 1;
			myCont->s_direct3dImmediateContext->VSSetConstantBuffers(s_type, bufferCount, &s_constantBuffer);
			myCont->s_direct3dImmediateContext->PSSetConstantBuffers(s_type, bufferCount, &s_constantBuffer);
		}

		void ConstantBuffer::Update(void* i_Data)
		{
			void* memoryToWriteTo = NULL;
			{
				D3D11_MAPPED_SUBRESOURCE mappedSubResource;
				{
					
					const unsigned int noSubResources = 0;
					const D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
					const unsigned int noFlags = 0;
					const HRESULT result = myCont->s_direct3dImmediateContext->Map(s_constantBuffer, noSubResources, mapType, noFlags, &mappedSubResource);
					if (SUCCEEDED(result))
					{
						memoryToWriteTo = mappedSubResource.pData;
					}
					else
					{
						ASSERT(false);
					}
				}
			}
			if (memoryToWriteTo)
			{
				memcpy(memoryToWriteTo, i_Data, s_BufferSize);
				const unsigned int noSubResources = 0;
				myCont->s_direct3dImmediateContext->Unmap(s_constantBuffer, noSubResources);
				memoryToWriteTo = NULL;
			}
		}

		bool ConstantBuffer::CleanUp()
		{
			bool wereThereErrors = false;
			
			if (s_constantBuffer)
			{
				s_constantBuffer->Release();
				s_constantBuffer = NULL;
			}

			return !wereThereErrors;
		}
	}
}