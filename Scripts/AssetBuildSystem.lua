local s_AuthoredAssetDir, s_BinDir, s_BuiltAssetDir
do
	do
		local key = "AuthoredAssetDir"
		s_AuthoredAssetDir = os.getenv( key )
		if not s_AuthoredAssetDir then
			error( "The environment variable \"" .. key .. "\" doesn't exist" )
		end
	end
	do
		local key = "BinDir"
		s_BinDir = os.getenv( key )
		if not s_BinDir then
			error( "The environment variable \"" .. key .. "\" doesn't exist" )
		end
	end
	do
		local key = "BuiltAssetDir"
		s_BuiltAssetDir = os.getenv( key )
		if not s_BuiltAssetDir then
			error( "The environment variable \"" .. key .. "\" doesn't exist" )
		end
	end
end

local cbAssetTypeInfo = {}
do
	cbAssetTypeInfo.__index = cbAssetTypeInfo
end

local assetTypeInfos = {}
local function NewAssetTypeInfo( i_assetTypeInfoKey, io_newAssetTypeInfoTable )
	local stackLevel = 2

	if not assetTypeInfos[i_assetTypeInfoKey] then
		if type( io_newAssetTypeInfoTable ) == "table" then
			setmetatable( io_newAssetTypeInfoTable, cbAssetTypeInfo )
			io_newAssetTypeInfoTable.type = i_assetTypeInfoKey
			assetTypeInfos[i_assetTypeInfoKey] = io_newAssetTypeInfoTable
		else
			error( "The new asset type info must be a table (instead of a " .. type( io_newAssetTypeInfoTable ) .. ")", stackLevel )
		end
	else
		if type( i_assetTypeInfoKey ) == "string" then
			error( "An asset type info named \"" .. i_assetTypeInfoKey .. "\" has already been defined", stackLevel )
		else
			error( "An asset type info has already been defined with the given key", stackLevel )
		end
	end
end

local registeredAssetsToBuild = {}

local function RegisterAssetToBeBuilt( i_sourceAssetRelativePath, i_assetType, i_optionalCommandLineArguments )
	local assetTypeInfo
	do
		if type( i_assetType ) == "string" then
			assetTypeInfo = assetTypeInfos[i_assetType]
			if not assetTypeInfo then
				error( "The source asset \"" .. tostring( i_sourceAssetRelativePath )
					.. "\" can't be registered to be built with the invalid asset type \"" .. i_assetType .. "\"" )
			end
		elseif getmetatable( i_assetType ) == cbAssetTypeInfo then
			assetTypeInfo = i_assetType
		else
			error( "The source asset \"" .. tostring( i_sourceAssetRelativePath )
				.. "\" can't be registered to be built with an invalid asset type that is a " .. type( i_assetType ) )
		end
	end
	local arguments
	do
		arguments = i_optionalCommandLineArguments or {}
		if type( arguments ) ~= "table" then
			error( "The source asset \"" .. tostring( i_sourceAssetRelativePath )
				.. "\" can't be registered to be built with optional command line arguments of type " .. type( arguments ) )
		end
	end
	local registrationInfo
	do
		local uniquePath = tostring( i_sourceAssetRelativePath )
		do
			uniquePath = uniquePath:lower()	
			uniquePath = uniquePath:gsub( "[/\\]+", "/" )	
		end
		registrationInfo = registeredAssetsToBuild[uniquePath]
		if not registrationInfo then
			registrationInfo = { path = uniquePath, assetTypeInfo = assetTypeInfo, arguments = arguments }
			registeredAssetsToBuild[uniquePath] = registrationInfo
			registeredAssetsToBuild[#registeredAssetsToBuild + 1] = registrationInfo
			assetTypeInfo.RegisterReferencedAssets( uniquePath )
		else
			if assetTypeInfo ~= registrationInfo.assetTypeInfo then
				error( "The source asset \"" .. tostring( i_sourceAssetRelativePath ) .. "\" can't be registered with type \"" .. assetTypeInfo.type
					.. "\" because it was already registered with type \"" .. registrationInfo.assetTypeInfo.type .. "\"" )
			end
			if #arguments == #registrationInfo.arguments then
				for i, argument in ipairs( arguments ) do
					if arguments[i] ~= registrationInfo.arguments[i] then
						error( "The source asset \"" .. tostring( i_sourceAssetRelativePath ) .. "\" can't be registered with a different argument ("
							.. tostring( argument ) .. ") than it was already registered with (" .. tostring( registrationInfo.argument ) .. ")" )
					end
				end
			else
				error( "The source asset \"" .. tostring( i_sourceAssetRelativePath ) .. "\" can't be registered with a different number of arguments ("
					.. tostring( #arguments ) .. ") than it was already registered with (" .. #registrationInfo.arguments .. ")" ) 
			end
		end
	end
end

function cbAssetTypeInfo.GetBuilderRelativePath()
end

function cbAssetTypeInfo.ConvertSourceRelativePathToBuiltRelativePath( i_sourceRelativePath )
	local relativeDirectory, file = i_sourceRelativePath:match( "(.-)([^/\\]+)$" )
	local fileName, extensionWithPeriod = file:match( "([^%.]+)(.*)" )
	return relativeDirectory .. fileName .. extensionWithPeriod
end

function cbAssetTypeInfo.RegisterReferencedAssets( i_sourceRelativePath )
end

function cbAssetTypeInfo.ShouldTargetBeBuilt( i_lastWriteTime_builtAsset )
	return false
end

NewAssetTypeInfo( "meshes",
	{
		GetBuilderRelativePath = function ()
	         return "MeshBuilder.exe"
        end,
		ConvertSourceRelativePathToBuiltRelativePath = function( i_sourceRelativePath )
			i_sourceRelativePath = i_sourceRelativePath:gsub("lua","bin")
			return i_sourceRelativePath
		end,
	}
)

NewAssetTypeInfo( "shaders",
	{
		GetBuilderRelativePath = function()
			return "ShaderBuilder.exe"
		end,
	}
)

NewAssetTypeInfo( "effects",
	{
		GetBuilderRelativePath = function ()
	         return "EffectBuilder.exe"
        end,
		ConvertSourceRelativePathToBuiltRelativePath = function( i_sourceRelativePath )
			i_sourceRelativePath = i_sourceRelativePath:gsub("effect","bin")
			return i_sourceRelativePath
		end,
		RegisterReferencedAssets = function( i_sourceRelativePath )
			local sourceAbsolutePath = s_AuthoredAssetDir .. i_sourceRelativePath
			if DoesFileExist( sourceAbsolutePath ) then
				local effect = dofile( sourceAbsolutePath )
				local vertexpath = effect.vertexshaderpath
				local fragmentpath = effect.fragmentshaderpath
				RegisterAssetToBeBuilt( vertexpath, "shaders", { "vertex" } )
				RegisterAssetToBeBuilt( fragmentpath, "shaders", { "fragment" } )
			end
		end,
	}
)

NewAssetTypeInfo( "textures",
	{
		GetBuilderRelativePath = function ()
	         return "TextureBuilder.exe"
        end,
		ConvertSourceRelativePathToBuiltRelativePath = function( i_sourceRelativePath )
			i_sourceRelativePath = i_sourceRelativePath:gsub("bmp","dds")
			i_sourceRelativePath = i_sourceRelativePath:gsub("jpg","dds")
			i_sourceRelativePath = i_sourceRelativePath:gsub("png","dds")
			i_sourceRelativePath = i_sourceRelativePath:gsub("tga","dds")
			return i_sourceRelativePath
		end,
	}
)

NewAssetTypeInfo( "materials",
	{
		GetBuilderRelativePath = function ()
	         return "MaterialBuilder.exe"
        end,
		ConvertSourceRelativePathToBuiltRelativePath = function( i_sourceRelativePath )
			i_sourceRelativePath = i_sourceRelativePath:gsub("mes","bin")
			return i_sourceRelativePath
		end,
		RegisterReferencedAssets = function( i_sourceRelativePath )
			local sourceAbsolutePath = s_AuthoredAssetDir .. i_sourceRelativePath
			if DoesFileExist( sourceAbsolutePath ) then
				local mater = dofile( sourceAbsolutePath )
				local effpath = mater.effectpath
				local texpath = mater.texturepath
				RegisterAssetToBeBuilt( effpath, "effects", { "effect" } )
				RegisterAssetToBeBuilt( texpath, "textures", { "texture" } )
			end
		end,
	}
)

local function BuildAsset( i_assetInfo )
	local assetTypeInfo = i_assetInfo.assetTypeInfo
	local path_source = s_AuthoredAssetDir .. i_assetInfo.path
	local path_target = s_BuiltAssetDir .. assetTypeInfo.ConvertSourceRelativePathToBuiltRelativePath( i_assetInfo.path )
	if not DoesFileExist( path_source ) then
		OutputErrorMessage( "The source asset doesn't exist", path_source )
		return false
	end

	local path_builder
	do
		local path_builder_relative = assetTypeInfo.GetBuilderRelativePath()
		if type( path_builder_relative ) == "string" then
			path_builder = s_BinDir .. path_builder_relative
			if not DoesFileExist( path_builder ) then
				OutputErrorMessage( "The builder \"" .. path_builder .. "\" doesn't exist", path_source )
				return false
			end
		else
			OutputErrorMessage( "The asset type info for \"" .. assetTypeInfo.type .. "\" must return a string from GetBuilderRelativePath()", path_source )
			return false
		end
	end

	local shouldTargetBeBuilt
	do
		local doesTargetExist = DoesFileExist( path_target )
		if doesTargetExist then
			local lastWriteTime_source = GetLastWriteTime( path_source )
			local lastWriteTime_target = GetLastWriteTime( path_target )
			shouldTargetBeBuilt = lastWriteTime_source > lastWriteTime_target
			if not shouldTargetBeBuilt then
				local lastWriteTime_builder = GetLastWriteTime( path_builder )
				shouldTargetBeBuilt = lastWriteTime_builder > lastWriteTime_target
				if not shouldTargetBeBuilt then
					shouldTargetBeBuilt = assetTypeInfo.ShouldTargetBeBuilt( lastWriteTime_target )
				end
			end
		else
			shouldTargetBeBuilt = true;
		end
	end

	if shouldTargetBeBuilt then
		CreateDirectoryIfNecessary( path_target )
		do
			local command = "\"" .. path_builder .. "\""
			local arguments = "\"" .. path_source .. "\" \"" .. path_target .. "\""
			if #i_assetInfo.arguments > 0 then
				arguments = arguments .. " " .. table.concat( i_assetInfo.arguments, " " )
			end
			local commandLine = command .. " " .. arguments
			local result, exitCode = ExecuteCommand( commandLine )
			if result then
				if exitCode == 0 then
					print( "Built " .. path_source )
					return true, exitCode
				else
					OutputErrorMessage( "The command " .. commandLine .. " failed with exit code " .. tostring( exitCode ), path_source )
				end
			else
				OutputErrorMessage( "The command " .. commandLine .. " couldn't be executed: " .. tostring( exitCode ), path_source )
			end

			if DoesFileExist( path_target ) then
				InvalidateLastWriteTime( path_target )
			end

			return false
		end
	else
		return true
	end
end

local function BuildAssets( i_path_assetsToBuild, i_stackLevelOfCaller )
	local wereThereErrors = false
	local assetsToBuild
	if DoesFileExist( i_path_assetsToBuild ) then
		assetsToBuild = dofile( i_path_assetsToBuild )
		if type( assetsToBuild ) ~= "table" then
			OutputErrorMessage( "The assets to build file (\"" .. i_path_assetsToBuild .. "\" must return a table", i_path_assetsToBuild )
		end
	else
		OutputErrorMessage( "The path to the list of assets to build (\"" .. i_path_assetsToBuild .. "\") doesn't exist" )
		return false
	end

	registeredAssetsToBuild = {}	
	for assetType, assetsToBuild_specificType in pairs( assetsToBuild ) do
		local assetTypeInfo = assetTypeInfos[assetType]
		if assetTypeInfo then
			for i, assetToBuild in ipairs( assetsToBuild_specificType ) do
				if type( assetToBuild ) == "string" then
					RegisterAssetToBeBuilt( assetToBuild, assetTypeInfo )
				elseif type( assetToBuild ) == "table" then
					RegisterAssetToBeBuilt( assetToBuild.path, assetTypeInfo, assetToBuild.arguments )
				else
					wereThereErrors = true
					OutputErrorMessage( "The asset #" .. tostring( i ) .. " defined to be built for \"" .. assetType
						.. "\" is a " .. type( assetToBuild ), i_path_assetsToBuild )
				end
			end
		else
			wereThereErrors = true
			OutputErrorMessage( "No asset type info has been defined for \"" .. tostring( assetType ) .. "\"", i_path_assetsToBuild )
		end
	end

	for i, assetInfo in ipairs( registeredAssetsToBuild ) do
		if not BuildAsset( assetInfo ) then
			wereThereErrors = true
		end
	end

	return not wereThereErrors
end

do
	local path_assetsToBuild = ...
	if path_assetsToBuild then
		return BuildAssets( path_assetsToBuild )
	else
	end
end

function ConvertSourceRelativePathToBuiltRelativePath( i_sourceRelativePath, i_assetType )
	local assetTypeInfo
	do
		if type( i_assetType ) == "string" then
			assetTypeInfo = assetTypeInfos[i_assetType]
			if not assetTypeInfo then
				return false, "\"" .. i_assetType .. "\" isn't a valid registered asset type"
			end
		elseif getmetatable( i_assetType ) == cbAssetTypeInfo then
			assetTypeInfo = i_assetType
		else
			return false, "The source relative path can't be converted with an invalid asset type that is a " .. type( i_assetType )
		end
	end
	local result, returnValue = pcall( assetTypeInfo.ConvertSourceRelativePathToBuiltRelativePath, i_sourceRelativePath )
	if result then
		return true, returnValue
	else
		return false, returnValue
	end
end