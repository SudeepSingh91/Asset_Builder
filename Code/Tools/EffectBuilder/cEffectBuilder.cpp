#include "cEffectBuilder.h"

#include <sstream>
#include <fstream>
#include "../AssetBuildLibrary/UtilityFunctions.h"

namespace
{
	bool LoadFile(const char* i_path, std::ofstream& i_binFile);
	bool LoadTableValues(lua_State& io_luaState, std::ofstream& i_binFile);
	bool LoadRenderStates(lua_State& io_luaState, std::ofstream& i_binFile);
}

bool eae6320::AssetBuild::cEffectBuilder::Build(const std::vector<std::string>&)
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
				eae6320::AssetBuild::OutputErrorMessage("Failed to create a new Lua state");
				goto OnExit;
			}
		}

		const int stackTopBeforeLoad = lua_gettop(luaState);
		{
			const int luaResult = luaL_loadfile(luaState, i_path);
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
				eae6320::AssetBuild::OutputErrorMessage(lua_tostring(luaState, -1));
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
						eae6320::AssetBuild::OutputErrorMessage("Asset files must return a table");
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					eae6320::AssetBuild::OutputErrorMessage("Asset files must return a single table");
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				eae6320::AssetBuild::OutputErrorMessage(lua_tostring(luaState, -1));
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
		if (!LoadRenderStates(io_luaState, i_binFile))
			return false;
		
		const char* key = "vertexshaderpath";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (lua_isnil(&io_luaState, -1))
		{
			eae6320::AssetBuild::OutputErrorMessage("No value for vertex shader path");
			lua_pop(&io_luaState, 1);
			return false;
		}
		if (lua_type(&io_luaState, -1) != LUA_TSTRING)
		{
			eae6320::AssetBuild::OutputErrorMessage("The value for vertex shader path must be a string");
			lua_pop(&io_luaState, 1);
			return false;
		}
		{
			const char* value = lua_tostring(&io_luaState, -1);

			std::string vertexpath = "";
			std::string errormsg = " ";

			eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(value, "shaders", vertexpath, &errormsg);

			vertexpath = "data/" + vertexpath + "\0";

			uint16_t size = static_cast<uint16_t>(strlen(vertexpath.c_str()));
			size++;

			char* csize = reinterpret_cast<char*>(&size);

			i_binFile.write(csize, sizeof(uint16_t));
			i_binFile.write(vertexpath.c_str(), size);

			lua_pop(&io_luaState, 1);
		}

		key = "fragmentshaderpath";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (lua_isnil(&io_luaState, -1))
		{
			eae6320::AssetBuild::OutputErrorMessage("No value for fragment shader path");
			lua_pop(&io_luaState, 1);
			return false;
		}
		if (lua_type(&io_luaState, -1) != LUA_TSTRING)
		{
			eae6320::AssetBuild::OutputErrorMessage("The value for fragment shader path must be a string");
			lua_pop(&io_luaState, 1);
			return false;
		}
		{
			const char* value = lua_tostring(&io_luaState, -1);

			std::string fragmentpath = "";
			std::string errormsg = " ";

			eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(value, "shaders", fragmentpath, &errormsg);

			fragmentpath = "data/" + fragmentpath + "\0";

			uint16_t size = static_cast<uint16_t>(strlen(fragmentpath.c_str()));
			size++;

			char* csize = reinterpret_cast<char*>(&size);
			i_binFile.write(fragmentpath.c_str(), size);

			lua_pop(&io_luaState, 1);
		}

		return true;
	}

	bool LoadRenderStates(lua_State& io_luaState, std::ofstream& i_binFile)
	{
		const char* const key = "renderstates";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (lua_istable(&io_luaState, -1))
		{

			uint8_t alphabits = 0;
			uint8_t depthbits = 0;
			uint8_t bothsidebits = 0;
			
			const char* const key1 = "alphatransparency";
			lua_pushstring(&io_luaState, key1);
			lua_gettable(&io_luaState, -2);

			if (!lua_isnil(&io_luaState, -1))
			{
				if (lua_isboolean(&io_luaState, -1))
				{
					int isalpha = lua_toboolean(&io_luaState, -1);
					if(isalpha == 1)
						alphabits = 1;
				}
			}

			lua_pop(&io_luaState, 1);

			const char* const key2 = "depthbuffering";
			lua_pushstring(&io_luaState, key2);
			lua_gettable(&io_luaState, -2);

			if (!lua_isnil(&io_luaState, -1))
			{
				if (lua_isboolean(&io_luaState, -1))
				{
					int isdepthbuffer = lua_toboolean(&io_luaState, -1);
					if (isdepthbuffer == 1)
						depthbits = 2;
				}
			}

			lua_pop(&io_luaState, 1);

			const char* const key3 = "drawbothsides";
			lua_pushstring(&io_luaState, key3);
			lua_gettable(&io_luaState, -2);

			if (!lua_isnil(&io_luaState, -1))
			{
				if (lua_isboolean(&io_luaState, -1))
				{
					int isbothsides = lua_toboolean(&io_luaState, -1);
					if (isbothsides == 1)
						bothsidebits = 4;
				}
			}

			lua_pop(&io_luaState, 1);

			uint8_t sum = alphabits + depthbits + bothsidebits;

			char* csize = reinterpret_cast<char*>(&sum);
			i_binFile.write(csize, sizeof(uint8_t));

			lua_pop(&io_luaState, 1);
			return true;
		}
		else
		{
			lua_pop(&io_luaState, 1);
			return false;
		}
	}
}