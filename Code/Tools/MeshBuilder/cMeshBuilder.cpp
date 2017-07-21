#include "cMeshBuilder.h"
#include <sstream>
#include <fstream>
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"

namespace
{
	struct sVertex
	{
		float x, y, z;
		float u, v;
		uint8_t r, g, b, a;
	};
	
	bool LoadFile(const char* i_path, std::ofstream& i_binFile);
	bool LoadTableValues(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadTableValues_vertices(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadTableValues_textures(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadTableValues_indices(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadTableValues_colors(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadTableValues_vertices_values(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadTableValues_textures_values(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadTableValues_indices_values(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadTableValues_color_values(lua_State& io_luaState, std::ofstream& i_binFile);

	sVertex* vertexbuffer = NULL;
}


bool Engine::AssetBuild::cMeshBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;
	{
		std::string errorMessage;

		std::ofstream binFile(m_path_target, std::ofstream::binary);

		if (!LoadFile(m_path_source, binFile))
			wereThereErrors = true;
	}

	return !wereThereErrors;
}

namespace
{
	bool LoadFile(const char* i_path, std::ofstream& i_binFile)
	{
		bool wereThereErrors = false;

		lua_State* luaState = NULL;
		{
			luaState = luaL_newstate();
			if (!luaState)
			{
				wereThereErrors = true;
				Engine::AssetBuild::OutputErrorMessage("Failed to create a new Lua state");
				goto OnExit;
			}
		}

		const int stackTopBeforeLoad = lua_gettop(luaState);
		{
			const int luaResult = luaL_loadfile(luaState, i_path);
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
				Engine::AssetBuild::OutputErrorMessage(lua_tostring(luaState, -1));
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}
		
		{
			const int argumentCount = 0;
			const int returnValueCount = LUA_MULTRET;	
			const int noMessageHandler = 0;
			const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
			if (luaResult == LUA_OK)
			{
				const int returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
				if (returnedValueCount == 1)
				{
					if (!lua_istable(luaState, -1))
					{
						wereThereErrors = true;
						Engine::AssetBuild::OutputErrorMessage("Asset files must return a table");
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					Engine::AssetBuild::OutputErrorMessage("Asset files must return a single table");
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				Engine::AssetBuild::OutputErrorMessage(lua_tostring(luaState, -1));
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}

		if (!LoadTableValues(*luaState, i_binFile))
		{
			wereThereErrors = true;
		}

		lua_pop(luaState, 1);

	OnExit:

		if (luaState)
		{
			lua_close(luaState);
			luaState = NULL;
		}

		return !wereThereErrors;
	}

	bool LoadTableValues(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		sVertex* vertexbuffer = NULL;
		
		if (!LoadTableValues_vertices(io_luaState, i_binFile))
		{
			return false;
		}
		if (!LoadTableValues_textures(io_luaState, i_binFile))
		{
			return false;
		}
		if (!LoadTableValues_indices(io_luaState, i_binFile))
		{
			return false;
		}

		if (vertexbuffer)
			delete vertexbuffer;

		return true;
	}

	bool LoadTableValues_vertices(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		bool wereThereErrors = false;
		const char* const key = "vertices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_istable(&io_luaState, -1))
		{
			if (!LoadTableValues_vertices_values(io_luaState, i_binFile))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			Engine::AssetBuild::OutputErrorMessage("The value at %d must be a table ", key);
			goto OnExit;
		}

	OnExit:

		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadTableValues_textures(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		bool wereThereErrors = false;
		const char* const key = "texturecoordinates";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_istable(&io_luaState, -1))
		{
			if (!LoadTableValues_textures_values(io_luaState, i_binFile))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			Engine::AssetBuild::OutputErrorMessage("The value at %d must be a table ", key);
			goto OnExit;
		}

	OnExit:

		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadTableValues_indices(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		bool wereThereErrors = false;
		const char* const key = "indices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_istable(&io_luaState, -1))
		{
			if (!LoadTableValues_indices_values(io_luaState, i_binFile))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			Engine::AssetBuild::OutputErrorMessage("The value at %d must be a table ", key);
			goto OnExit;
		}

	OnExit:

		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadTableValues_vertices_values(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		uint16_t verticeCount = luaL_len(&io_luaState, -1);
		char* vertexcountbuf = reinterpret_cast<char*>(&verticeCount);
		i_binFile.write(vertexcountbuf, sizeof(uint16_t));

		vertexbuffer = new sVertex[verticeCount];
		for (int i = 1; i <= verticeCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
				lua_pushinteger(&io_luaState, 1);
				lua_gettable(&io_luaState, -2);
				vertexbuffer[i - 1].x = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 2);
				lua_gettable(&io_luaState, -2);
				vertexbuffer[i - 1].y = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 3);
				lua_gettable(&io_luaState, -2);
				vertexbuffer[i - 1].z = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 4);
				lua_gettable(&io_luaState, -2);
				float temp = static_cast<float>(lua_tonumber(&io_luaState, -1) * 255.0f);
				vertexbuffer[i - 1].r = static_cast<uint8_t>(temp);
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 5);
				lua_gettable(&io_luaState, -2);
				temp = static_cast<float>(lua_tonumber(&io_luaState, -1) * 255.0f);
				vertexbuffer[i - 1].g = static_cast<uint8_t>(temp);
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 6);
				lua_gettable(&io_luaState, -2);
				temp = static_cast<float>(lua_tonumber(&io_luaState, -1) * 255.0f);
				vertexbuffer[i - 1].b = static_cast<uint8_t>(temp);
				lua_pop(&io_luaState, 1);
				vertexbuffer[i - 1].a = static_cast<uint8_t>(255.0f);
			}
			lua_pop(&io_luaState, 1);
		}

		return true;
	}

	bool LoadTableValues_textures_values(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		uint16_t verticeCount = luaL_len(&io_luaState, -1);
		char* vertexcountbuf = reinterpret_cast<char*>(&verticeCount);

		for (int i = 1; i <= verticeCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
#if defined( PLATFORM_D3D )
				lua_pushinteger(&io_luaState, 1);
				lua_gettable(&io_luaState, -2);
				vertexbuffer[i - 1].u = static_cast<float>(lua_tonumber(&io_luaState, -1));
				vertexbuffer[i - 1].u -= 1.0f;
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 2);
				lua_gettable(&io_luaState, -2);
				vertexbuffer[i - 1].v = static_cast<float>(lua_tonumber(&io_luaState, -1));
				vertexbuffer[i - 1].v -= 1.0f;
				lua_pop(&io_luaState, 1);
#elif defined( PLATFORM_GL )
				lua_pushinteger(&io_luaState, 1);
				lua_gettable(&io_luaState, -2);
				vertexbuffer[i - 1].u = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 2);
				lua_gettable(&io_luaState, -2);
				vertexbuffer[i - 1].v = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
#endif
			}
			lua_pop(&io_luaState, 1);
		}

		char* buffer = reinterpret_cast<char*>(vertexbuffer);
		i_binFile.write(buffer, sizeof(sVertex) * verticeCount);

		return true;
	}

	bool LoadTableValues_indices_values(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		uint16_t indiceCount = luaL_len(&io_luaState, -1);
		char* indexcountbuf = reinterpret_cast<char*>(&indiceCount);
		i_binFile.write(indexcountbuf, sizeof(uint16_t));

		uint16_t* indexbuffer = new uint16_t[indiceCount];
		int j = 0;
#if defined( PLATFORM_D3D )
		for (int i = indiceCount; i >= 1; --i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			indexbuffer[indiceCount - i] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
			lua_pop(&io_luaState, 1);
		}
#elif defined( PLATFORM_GL )
		for (int i = 1; i <= indiceCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			indexbuffer[i - 1] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
			lua_pop(&io_luaState, 1);
		}
#endif

		char* buf = reinterpret_cast<char*>(indexbuffer);
		i_binFile.write(buf, indiceCount * sizeof(uint16_t));

		return true;
	}

	bool LoadTableValues_colors(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		bool wereThereErrors = false;
		const char* const key = "color";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_istable(&io_luaState, -1))
		{
			if (!LoadTableValues_color_values(io_luaState, i_binFile))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			Engine::AssetBuild::OutputErrorMessage("The value at %d must be a table ", key);
			goto OnExit;
		}

	OnExit:

		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadTableValues_color_values(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		uint16_t colorCount = luaL_len(&io_luaState, -1);
		char* colorcountbuf = reinterpret_cast<char*>(&colorCount);
		i_binFile.write(colorcountbuf, sizeof(uint16_t));

		uint8_t* colorbuffer = new uint8_t[colorCount * 4];
		int j = 0;
		for (int i = 1; i <= colorCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			if (lua_istable(&io_luaState, -1))
			{
				lua_pushinteger(&io_luaState, 1);
				lua_gettable(&io_luaState, -2);
				float temp = static_cast<float>(lua_tonumber(&io_luaState, -1)  * 255.0f);
				colorbuffer[j] = static_cast<uint8_t>(temp);
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 2);
				lua_gettable(&io_luaState, -2);
				temp = static_cast<float>(lua_tonumber(&io_luaState, -1)  * 255.0f);
				colorbuffer[j + 1] = static_cast<uint8_t>(temp);
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 3);
				lua_gettable(&io_luaState, -2);
				temp = static_cast<float>(lua_tonumber(&io_luaState, -1)  * 255.0f);
				colorbuffer[j + 2] = static_cast<uint8_t>(temp);
				lua_pop(&io_luaState, 1);
				colorbuffer[j + 3] = static_cast<uint8_t>(255.0f);
			}
			lua_pop(&io_luaState, 1);
			j += 4;
		}

		char* buf = reinterpret_cast<char*>(colorbuffer);
		i_binFile.write(buf, colorCount * 4 * sizeof(uint8_t));

		return true;
	}
}