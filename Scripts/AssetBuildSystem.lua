--[[
	This file contains the logic for building assets
]]

-- Static Data Initialization
--===========================

local s_AuthoredAssetDir, s_BinDir, s_BuiltAssetDir
do
	-- AuthoredAssetDir
	do
		local key = "AuthoredAssetDir"
		s_AuthoredAssetDir = os.getenv( key )
		if not s_AuthoredAssetDir then
			error( "The environment variable \"" .. key .. "\" doesn't exist" )
		end
	end
	-- BinDir
	do
		local key = "BinDir"
		s_BinDir = os.getenv( key )
		if not s_BinDir then
			error( "The environment variable \"" .. key .. "\" doesn't exist" )
		end
	end
	-- BuiltAssetDir
	do
		local key = "BuiltAssetDir"
		s_BuiltAssetDir = os.getenv( key )
		if not s_BuiltAssetDir then
			error( "The environment variable \"" .. key .. "\" doesn't exist" )
		end
	end
end

-- Asset Types
--============

-- To make something like a C++ class in Lua requires a table:
local cbAssetTypeInfo = {}
do
	-- And the use of something called "metatables"
	-- to give inheritance behavior.
	-- You don't need to understand the details for our class,
	-- but the following line means that any table
	-- that has cbAssetTypeInfo assigned as its metatable
	-- will do the following when indexed:
	--	* If the table itself has the key being indexed it will be used
	--	* Otherwise the key of cbAssetTypeInfo will be used
	cbAssetTypeInfo.__index = cbAssetTypeInfo
end

-- Each asset type that you define will go in a table that can be accessed by a key:
local assetTypeInfos = {}
-- You can define a new asset type and add it to the list using this function.
-- The table that you provide must have the required functions shown above.
local function NewAssetTypeInfo( i_assetTypeInfoKey, io_newAssetTypeInfoTable )
	local stackLevel = 2

	if not assetTypeInfos[i_assetTypeInfoKey] then
		if type( io_newAssetTypeInfoTable ) == "table" then
			-- Setting the base class cbAssetTypeInfo as the metatable
			-- of the given table causes the given table to inherit from cbAssetTypeInfo
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

-- In order to be built an asset must be "registered"
local registeredAssetsToBuild = {}
--
local function RegisterAssetToBeBuilt( i_sourceAssetRelativePath, i_assetType, i_optionalCommandLineArguments )
	-- Get the asset type info
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
	-- Get the arguments
	local arguments
	do
		arguments = i_optionalCommandLineArguments or {}
		if type( arguments ) ~= "table" then
			error( "The source asset \"" .. tostring( i_sourceAssetRelativePath )
				.. "\" can't be registered to be built with optional command line arguments of type " .. type( arguments ) )
		end
	end
	-- Check if this source asset has already been registered
	local registrationInfo
	do
		-- Create a unique version of the path
		local uniquePath = tostring( i_sourceAssetRelativePath )
		do
			uniquePath = uniquePath:lower()	-- lower case
			uniquePath = uniquePath:gsub( "[/\\]+", "/" )	-- single forward slash
		end
		registrationInfo = registeredAssetsToBuild[uniquePath]
		if not registrationInfo then
			-- If this source asset hasn't been registered yet then register it now
			registrationInfo = { path = uniquePath, assetTypeInfo = assetTypeInfo, arguments = arguments }
			-- (This table is simultaneously used as a dictionary and an array)
			registeredAssetsToBuild[uniquePath] = registrationInfo
			registeredAssetsToBuild[#registeredAssetsToBuild + 1] = registrationInfo
			-- And also register any assets that are referenced by this asset
			assetTypeInfo.RegisterReferencedAssets( uniquePath )
		else
			-- If this source asset has already been registered then the information must be identical
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

-- You will need to override the following function for every new asset type that you create
function cbAssetTypeInfo.GetBuilderRelativePath()
	-- This function should return the appropriate builder EXE
	-- for the specific asset type
end

-- If you want to change the relative path (including file extension) of built assets from their source assets
-- then you will need to override the following function
function cbAssetTypeInfo.ConvertSourceRelativePathToBuiltRelativePath( i_sourceRelativePath )
	local relativeDirectory, file = i_sourceRelativePath:match( "(.-)([^/\\]+)$" )
	local fileName, extensionWithPeriod = file:match( "([^%.]+)(.*)" )
	-- By default the relative paths are the same
	return relativeDirectory .. fileName .. extensionWithPeriod
end

-- You will need to override the following function for some new asset types, but not for all
function cbAssetTypeInfo.RegisterReferencedAssets( i_sourceRelativePath )
	-- Some asset types reference other assets
	-- (e.g. materials use textures and effects, and effects use shaders).
	-- This function registers any assets that are referenced by the given source asset.
	-- The base class does nothing,
	-- but you will have to override this function for some asset types.
end

-- You may need to override the following function for some new asset types, but not for many
function cbAssetTypeInfo.ShouldTargetBeBuilt( i_lastWriteTime_builtAsset )
	-- By default this returns false,
	-- because there are no special dependencies for this asset type
	-- that need to be taken into account
	return false
end

-- Mesh Asset Type
--------------------

NewAssetTypeInfo( "meshes",
	{
		GetBuilderRelativePath = function ()
	         return "MeshBuilder.exe"
        end,
		ConvertSourceRelativePathToBuiltRelativePath = function( i_sourceRelativePath )
			i_sourceRelativePath = i_sourceRelativePath:gsub("lua","bin")
			-- By default the relative paths are the same
			return i_sourceRelativePath
		end,
	}
)

-- Shader Asset Type
--------------------

NewAssetTypeInfo( "shaders",
	{
		-- This function is required for all asset types
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
			-- By default the relative paths are the same
			return i_sourceRelativePath
		end,
		RegisterReferencedAssets = function( i_sourceRelativePath )
			local sourceAbsolutePath = s_AuthoredAssetDir .. i_sourceRelativePath
			if DoesFileExist( sourceAbsolutePath ) then
				local effect = dofile( sourceAbsolutePath )
				local vertexpath = effect.vertexshaderpath
				local fragmentpath = effect.fragmentshaderpath
				-- EAE6320_TODO Get the shader paths from the effect table and then do something like:
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
			-- By default the relative paths are the same
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
			-- By default the relative paths are the same
			return i_sourceRelativePath
		end,
		RegisterReferencedAssets = function( i_sourceRelativePath )
			local sourceAbsolutePath = s_AuthoredAssetDir .. i_sourceRelativePath
			if DoesFileExist( sourceAbsolutePath ) then
				local mater = dofile( sourceAbsolutePath )
				local effpath = mater.effectpath
				local texpath = mater.texturepath
				-- EAE6320_TODO Get the shader paths from the effect table and then do something like:
				RegisterAssetToBeBuilt( effpath, "effects", { "effect" } )
				RegisterAssetToBeBuilt( texpath, "textures", { "texture" } )
			end
		end,
	}
)

-- Function Definitions
--=====================

local function BuildAsset( i_assetInfo )
	local assetTypeInfo = i_assetInfo.assetTypeInfo

	-- Get the absolute paths to the source and target
	-- (The "source" is the authored asset,
	-- and the "target" is the built asset that is ready to be used in-game)
	local path_source = s_AuthoredAssetDir .. i_assetInfo.path
	local path_target = s_BuiltAssetDir .. assetTypeInfo.ConvertSourceRelativePathToBuiltRelativePath( i_assetInfo.path )
	if not DoesFileExist( path_source ) then
		OutputErrorMessage( "The source asset doesn't exist", path_source )
		return false
	end

	-- Get the path to the builder
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

	-- Decide if the target needs to be built
	local shouldTargetBeBuilt
	do
		-- The simplest reason a target should be built is if it doesn't exist
		local doesTargetExist = DoesFileExist( path_target )
		if doesTargetExist then
			-- Even if the target exists it may be out-of-date.
			-- If the source has been modified more recently than the target
			-- then the target should be re-built.
			local lastWriteTime_source = GetLastWriteTime( path_source )
			local lastWriteTime_target = GetLastWriteTime( path_target )
			shouldTargetBeBuilt = lastWriteTime_source > lastWriteTime_target
			if not shouldTargetBeBuilt then
				-- Even if the target is newer than the source
				-- the builder EXE may have been updated
				-- which could result in a different target being made
				-- (e.g. if you fix a bug in the builder code)
				local lastWriteTime_builder = GetLastWriteTime( path_builder )
				shouldTargetBeBuilt = lastWriteTime_builder > lastWriteTime_target
				if not shouldTargetBeBuilt then
					-- There might be other dependencies specific to this asset type
					shouldTargetBeBuilt = assetTypeInfo.ShouldTargetBeBuilt( lastWriteTime_target )
				end
			end
		else
			shouldTargetBeBuilt = true;
		end
	end

	-- Build the target if necessary
	if shouldTargetBeBuilt then
		-- Create the target directory if necessary
		CreateDirectoryIfNecessary( path_target )
		-- Build
		do
			-- The command starts with the builder
			local command = "\"" .. path_builder .. "\""
			-- The source and target path must always be passed in
			local arguments = "\"" .. path_source .. "\" \"" .. path_target .. "\""
			-- Some asset types may have optional arguments
			if #i_assetInfo.arguments > 0 then
				arguments = arguments .. " " .. table.concat( i_assetInfo.arguments, " " )
			end
			-- Execute the command
			local commandLine = command .. " " .. arguments
			local result, exitCode = ExecuteCommand( commandLine )
			if result then
				if exitCode == 0 then
					-- Display a message for each asset
					print( "Built " .. path_source )
					-- Return the exit code for informational purposes since we have it
					return true, exitCode
				else
					-- The builder should already output a descriptive error message if there was an error
					-- (remember that you write the builder code,
					-- and so if the build process failed it means that _your_ code has returned an error code)
					-- but it can be helpful to still return an additional vague error message here
					-- in case there is a bug in the specific builder that doesn't output an error message
					OutputErrorMessage( "The command " .. commandLine .. " failed with exit code " .. tostring( exitCode ), path_source )
				end
			else
				-- If the command wasn't executed then the second return value is an error message
				OutputErrorMessage( "The command " .. commandLine .. " couldn't be executed: " .. tostring( exitCode ), path_source )
			end

			-- There's a chance that the builder already created the target file even though the build failed,
			-- in which case it will have a new time stamp and wouldn't get built again
			-- even though the process failed
			if DoesFileExist( path_target ) then
				-- Setting the time stamp to an invalid date in far in the past
				-- allows you to look at the generated file if you wish
				-- but still ensures that the build process will attempt to build it again
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

	-- Load the list of assets to build
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

	-- Register every asset that needs to be built
	registeredAssetsToBuild = {}	-- Clear the table
	-- Iterate through every type of asset in the file
	for assetType, assetsToBuild_specificType in pairs( assetsToBuild ) do
		-- In order for an asset of this type to be built
		-- an asset type info must have been defined
		local assetTypeInfo = assetTypeInfos[assetType]
		if assetTypeInfo then
			-- Iterate through every asset of this type
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

	-- Build every asset that was registered
	for i, assetInfo in ipairs( registeredAssetsToBuild ) do
		if not BuildAsset( assetInfo ) then
			wereThereErrors = true
		end
	end

	return not wereThereErrors
end

-- Entry Point
--============

do
	-- Arguments passed to an entire file are represented in Lua as three dots ("..."),
	-- which can be assigned to a comma-separated list
	local path_assetsToBuild = ...
	if path_assetsToBuild then
		-- If this file is being executed with arguments
		-- then it will build assets based on those arguments
		return BuildAssets( path_assetsToBuild )
	else
		-- If this file is being executed without arguments
		-- the global functions will be added to the Lua environment
		-- but no code will actually be executed
	end
end

function ConvertSourceRelativePathToBuiltRelativePath( i_sourceRelativePath, i_assetType )
	-- Get the asset type info
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
	-- Return the converted path
	local result, returnValue = pcall( assetTypeInfo.ConvertSourceRelativePathToBuiltRelativePath, i_sourceRelativePath )
	if result then
		return true, returnValue
	else
		return false, returnValue
	end
end