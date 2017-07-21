#ifndef CMYGAME_H
#define CMYGAME_H

#include <vector>

#include "../../Engine/Application/cbApplication.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/Effect.h"
#include "../../Engine/Graphics/Material.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/Math/cVector.h"
#include "../../Engine/Math/cQuaternion.h"
#include "../../Engine/Graphics/Camera.h"
#include "../../Engine/Graphics/cSprite.h"

#if defined( PLATFORM_WINDOWS )
#include "Resource Files/Resource.h"
#endif

namespace Engine
{
	class cMyGame : public Application::cbApplication
	{
	public:
		cMyGame();
		virtual ~cMyGame();

	private:
		virtual const char* GetPathToLogTo() const { return "Engine.log"; }
#if defined( PLATFORM_WINDOWS )
		virtual const char* GetMainWindowName() const
		{
			return "Sudeep's Game"
				" -- "
#if defined( Engine_PLATFORM_D3D )
				"Direct3D"
#elif defined( Engine_PLATFORM_GL )
				"OpenGL"
#endif
#ifdef _DEBUG
				" -- Debug"
#endif
				;
		}
		virtual const char* GetMainWindowClassName() const { return "[YOUR NAME]'s Engine Main Window Class"; }
		virtual const WORD* GetLargeIconId() const { static const WORD iconId_large = IDI_EAEGAMEPAD; return &iconId_large; }
		virtual const WORD* GetSmallIconId() const { static const WORD iconId_small = IDI_EAEGAMEPAD; return &iconId_small; }
#endif

		virtual bool Initialize();
		virtual void DrawMesh();
		virtual bool CleanUp();

		const int m_numMesh;
		const int m_numMat;

		Graphics::MeshData* m_meshData;
		Graphics::cSprite* spr1;
		Math::cVector* s_camPos;
		Math::cQuaternion* s_camOri;
		Camera::Camera* s_Camera;

		Engine::Graphics::Materials::Material* m_meshMat;
	};
}

#endif	
