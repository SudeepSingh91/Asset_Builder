#include "Graphics.h"

namespace Engine
{
	namespace Graphics
	{
		GraphicsData* GraphicsData::m_graphicsData = nullptr;

		GraphicsData* GraphicsData::GetGraphicsInst()
		{
			if (m_graphicsData == nullptr)
			{
				m_graphicsData = new GraphicsData;
			}

			return m_graphicsData;
		}

		void GraphicsData::Destroy()
		{
			if (m_graphicsData != nullptr)
			{
				delete m_graphicsData;
				m_graphicsData = nullptr;
			}
		}

		GraphicsData::~GraphicsData()
		{

		}
	}
}