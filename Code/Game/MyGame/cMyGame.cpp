#include "cMyGame.h"

Engine::cMyGame::cMyGame() : m_numMesh(8), m_numMat(4), m_meshData(nullptr), s_camPos(nullptr), s_camOri(nullptr), s_Camera(nullptr), m_meshMat(nullptr)
{

}

Engine::cMyGame::~cMyGame()
{

}

bool Engine::cMyGame::Initialize()
{
	Engine::Graphics::GraphicsData* graphicsData = Engine::Graphics::GraphicsData::GetGraphicsInst();

	const char* const path = "data/Meshes/Car.bin";
	const char* const path2 = "data/Meshes/Platform.bin";
	const char* const path3 = "data/Meshes/Cylinder.bin";

	const char* const matpath1 = "data/Materials/mat1.bin";
	const char* const matpath2 = "data/Materials/mat2.bin";
	const char* const matpath3 = "data/Materials/mat3.bin";
	const char* const matpath4 = "data/Materials/mat4.bin";

	m_meshData = new Engine::Graphics::MeshData[m_numMesh];
	for (int i = 0; i < m_numMesh; i++)
	{
		(m_meshData + i)->s_mesh = new Engine::Mesh::Mesh;
	}

	m_meshMat = new Engine::Graphics::Materials::Material[m_numMat];

	s_camPos = new Engine::Math::cVector(0.0f, 5.0f, 20.0f);
	s_camOri = new Engine::Math::cQuaternion;
	s_Camera = new Engine::Camera::Camera(*s_camPos, *s_camOri, Engine::Math::ConvertDegreesToRadians(60.0f), 0.1f, 100.0f);
	spr1 = new Engine::Graphics::cSprite(-0.3f, 0.3f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f);
	spr1->Initialize();
	bool wereThereErrors = false;

	if (!m_meshData->s_mesh->LoadFile(path))
	{
		wereThereErrors = true;
	}
	if (!(m_meshData + 1)->s_mesh->LoadFile(path2))
	{
		wereThereErrors = true;
	}
	if (!(m_meshData + 2)->s_mesh->LoadFile(path2))
	{
		wereThereErrors = true;
	}
	if (!(m_meshData + 3)->s_mesh->LoadFile(path3))
	{
		wereThereErrors = true;
	}
	if (!(m_meshData + 4)->s_mesh->LoadFile(path3))
	{
		wereThereErrors = true;
	}
	if (!(m_meshData + 5)->s_mesh->LoadFile(path3))
	{
		wereThereErrors = true;
	}
	if (!(m_meshData + 6)->s_mesh->LoadFile(path2))
	{
		wereThereErrors = true;
	}
	if (!(m_meshData + 7)->s_mesh->LoadFile(path2))
	{
		wereThereErrors = true;
	}
	if (!m_meshMat->Load(matpath1))
	{
		wereThereErrors = true;
	}
	if (!(m_meshMat + 1)->Load(matpath2))
	{
		wereThereErrors = true;
	}
	if (!(m_meshMat + 2)->Load(matpath3))
	{
		wereThereErrors = true;
	}
	if (!(m_meshMat + 3)->Load(matpath4))
	{
		wereThereErrors = true;
	}
	m_meshData->s_mat = m_meshMat;
	(m_meshData + 1)->s_mat = (m_meshMat + 1);
	(m_meshData + 2)->s_mat = (m_meshMat + 1);
	(m_meshData + 3)->s_mat = (m_meshMat + 2);
	(m_meshData + 4)->s_mat = (m_meshMat + 2);
	(m_meshData + 5)->s_mat = (m_meshMat + 2);
	(m_meshData + 6)->s_mat = (m_meshMat + 1);
	(m_meshData + 7)->s_mat = (m_meshMat + 1);
	graphicsData->SetCamera(*s_Camera);
	(m_meshData + 2)->s_pos.z((m_meshData + 2)->s_pos.z() - 24.0f);
	(m_meshData + 6)->s_pos.z((m_meshData + 6)->s_pos.z() - 48.0f);
	(m_meshData + 7)->s_pos.z((m_meshData + 7)->s_pos.z() - 72.0f);
	graphicsData->SetMesh(*(m_meshData + 2));
	(m_meshData + 3)->s_pos.z((m_meshData + 3)->s_pos.z() - 20.0f);
	(m_meshData + 3)->s_pos.x((m_meshData + 3)->s_pos.x() + 5.0f);
	graphicsData->SetMesh(*(m_meshData + 3));
	(m_meshData + 4)->s_pos.z((m_meshData + 4)->s_pos.z() - 35.0f);
	(m_meshData + 4)->s_pos.x((m_meshData + 4)->s_pos.x() - 5.0f);
	graphicsData->SetMesh(*(m_meshData + 4));
	(m_meshData + 5)->s_pos.z((m_meshData + 5)->s_pos.z() - 43.0f);
	(m_meshData + 5)->s_pos.x((m_meshData + 5)->s_pos.x() + 5.0f);
	graphicsData->SetMesh(*(m_meshData + 5));
	return !wereThereErrors;
}

void Engine::cMyGame::DrawMesh()
{
	Engine::Graphics::GraphicsData* graphicsData = Engine::Graphics::GraphicsData::GetGraphicsInst();

	float offset[2] = { 0.0f, 0.0f };
	{
		if (UserInput::IsKeyPressed('A'))
			offset[0] -= 4.0f;
		if (UserInput::IsKeyPressed('D'))
			offset[0] += 4.0f;
		offset[1] -= 60.0f;
	}
	{
		const float speed_unitsPerSecond = 0.2f;
		const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();

		offset[0] *= offsetModifier;
		offset[1] *= offsetModifier;
	}
	Engine::Math::cVector tempVec(0.0f, 0.0f, offset[1]);
	s_Camera->Move(tempVec);

	Engine::Math::cVector tempx(1.0f, 0.0f, 0.0f);
	Engine::Math::cVector tempy(0.0f, 1.0f, 0.0f);

	m_meshData->s_pos.z(m_meshData->s_pos.z() + offset[1]);
	m_meshData->s_pos.x(m_meshData->s_pos.x() + offset[0]);

	for (int i = 0; i < m_numMesh - 1; i++)
	{
		graphicsData->SetMesh(*(m_meshData + i));
	}

	graphicsData->SetSprite(spr1, (m_meshMat + 3));
	graphicsData->SetCamera(*s_Camera);
	graphicsData->RenderFrame();
}

bool Engine::cMyGame::CleanUp()
{
	bool wereThereErrors = false;

	for (int i = 0; i < m_numMesh; i++)
	{
		if ((m_meshData + i)->s_mesh && !(m_meshData + i)->s_mesh->CleanUp())
		{
			return false;
		}
	}

	delete[] m_meshData;
	delete[] m_meshMat;


	if (s_camPos)
	{
		delete s_camPos;
	}
	if (s_Camera)
	{
		delete s_Camera;
	}
	if (spr1)
	{
		spr1->CleanUp();
		delete spr1;
	}

	Engine::Graphics::GraphicsData::Destroy();

	return !wereThereErrors;
}
