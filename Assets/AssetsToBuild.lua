--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	-- The top-level table is a dictionary whose keys
	-- correspond to the asset types (inherited from cbAssetTypeInfo)
	-- that you have defined in AssetBuildSystem.lua

	meshes =
	{
		-- The actual assets can be defined simply as relative path strings (the common case)
		"Meshes/Car.lua",
		"Meshes/Platform.lua",
		"Meshes/Cylinder.lua",
	},
	materials =
	{
		"Materials/mat1.mes",
		"Materials/mat2.mes",
		"Materials/mat3.mes",
		"Materials/mat4.mes",
	},
	textures = 
	{
		"Textures/default.tga",
	},
}
