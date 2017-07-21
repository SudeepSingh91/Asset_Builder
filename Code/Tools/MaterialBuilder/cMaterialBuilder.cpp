#include "cMaterialBuilder.h"

#include <sstream>
#include <fstream>
#include "../AssetBuildLibrary/UtilityFunctions.h"

namespace
{
	bool LoadFile(const char* i_path, std::ofstream& i_binFile);
	bool LoadTableValues(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadConstantData(lua_State& io_luaState, std::ofstream& i_binFile);

	struct sMaterial
	{
		struct
		{
			float r, g, b, a;
		} g_color;

		sMaterial()
		{
			g_color.r = 1.0f;
			g_color.g = 1.0f;
			g_color.b = 1.0f;
			g_color.a = 1.0f;
		}
	};
}

bool Engine::AssetBuild::cMaterialBuilder::Build(const std::vector<std::string>&)
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
		if (!LoadConstantData(io_luaState, i_binFile))
			return false;
		
		{
			const char* key = "texturepath";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			if (lua_isnil(&io_luaState, -1))
			{
				std::string texpath = "data/Textures/default.bin\0";

				uint16_t size = static_cast<uint16_t>(strlen(texpath.c_str()));
				size++;

				char* csize = reinterpret_cast<char*>(&size);

				i_binFile.write(csize, sizeof(uint16_t));
				i_binFile.write(texpath.c_str(), size);

				lua_pop(&io_luaState, 1);
			}
			else
			{
				const char* value = lua_tostring(&io_luaState, -1);

				std::string texpath = "";
				std::string errormsg = " ";

				Engine::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(value, "textures", texpath, &errormsg);

				texpath = "data/" + texpath + "\0";

				uint16_t size = static_cast<uint16_t>(strlen(texpath.c_str()));
				size++;

				char* csize = reinterpret_cast<char*>(&size);

				i_binFile.write(csize, sizeof(uint16_t));
				i_binFile.write(texpath.c_str(), size);

				lua_pop(&io_luaState, 1);
			}
		}

		const char* key = "effectpath";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (lua_isnil(&io_luaState, -1))
		{
			Engine::AssetBuild::OutputErrorMessage("No value for effect path");
			lua_pop(&io_luaState, 1);
			return false;
		}
		if (lua_type(&io_luaState, -1) != LUA_TSTRING)
		{
			Engine::AssetBuild::OutputErrorMessage("The value for effect path must be a string");
			lua_pop(&io_luaState, 1);
			return false;
		}
		{
			const char* value = lua_tostring(&io_luaState, -1);

			std::string effpath = "";
			std::string errormsg = " ";

			Engine::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(value, "effects", effpath, &errormsg);

			effpath = "data/" + effpath + "\0";

			uint16_t size = static_cast<uint16_t>(strlen(effpath.c_str()));
			size++;

			i_binFile.write(effpath.c_str(), size);

			lua_pop(&io_luaState, 1);
		}
		return true;
	}

	bool LoadConstantData(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		bool wereThereErrors = false;
		
		const char* const key = "constantdata";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (lua_istable(&io_luaState, -1))
		{
			const char* const key1 = "color";
			lua_pushstring(&io_luaState, key1);
			lua_gettable(&io_luaState, -2);
			
			sMaterial* data = new sMaterial;

			if (lua_istable(&io_luaState, -1))
			{
				lua_pushinteger(&io_luaState, 1);
				lua_gettable(&io_luaState, -2);
				data->g_color.r = static_cast<float>(lua_tonumber(&io_luaState, -1) );
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 2);
				lua_gettable(&io_luaState, -2);
				data->g_color.g = static_cast<float>(lua_tonumber(&io_luaState, -1) );
				lua_pop(&io_luaState, 1);
				lua_pushinteger(&io_luaState, 3);
				lua_gettable(&io_luaState, -2);
				data->g_color.b = static_cast<float>(lua_tonumber(&io_luaState, -1) );
				lua_pop(&io_luaState, 1);
				char* cdata = reinterpret_cast<char*>(data);
				i_binFile.write(cdata, sizeof(sMaterial));
			}
			else
			{
				wereThereErrors = true;
				Engine::AssetBuild::OutputErrorMessage("Could not find data for color");
			}

			lua_pop(&io_luaState, 1);
		}
		else
		{
			sMaterial* data = new sMaterial;
			char* cdata = reinterpret_cast<char*>(data);
			i_binFile.write(cdata, sizeof(sMaterial));
			Engine::AssetBuild::OutputErrorMessage("Calling default constructor");
		}

		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
}