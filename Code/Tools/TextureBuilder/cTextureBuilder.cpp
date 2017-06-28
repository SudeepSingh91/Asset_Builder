// Header Files
//=============

#include "cTextureBuilder.h"

#include <codecvt>
#include <locale>
#include <string>
#include <utility>
#include "../../Engine/Math/Functions.h"
#include "../../External/DirectXTex/Includes.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"

// Helper Function Declarations
//=============================

namespace
{
	bool BuildTexture( const char *const i_path, DirectX::ScratchImage &io_sourceImageThatMayNotBeValidAfterThisCall,
		DirectX::ScratchImage &o_texture );
	bool LoadSourceImage( const char *const i_path, DirectX::ScratchImage &o_image );
	bool WriteTextureToFile( const char *const i_path, const DirectX::ScratchImage &i_texture );
}

// Inherited Implementation
//=========================

// Build
//------

bool eae6320::AssetBuild::cTextureBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;

	DirectX::ScratchImage sourceImage;
	DirectX::ScratchImage builtTexture;
	bool shouldComBeUninitialized = false;

	// Initialize COM
	{
		void* const thisMustBeNull = NULL;
		if ( SUCCEEDED( CoInitialize( thisMustBeNull ) ) )
		{
			shouldComBeUninitialized = true;
		}
		else
		{
			wereThereErrors = true;
			eae6320::AssetBuild::OutputErrorMessage( "DirextXTex couldn't be used because COM couldn't be initialized", m_path_source );
			goto OnExit;
		}
	}
	// Load the source image
	if ( !LoadSourceImage( m_path_source, sourceImage ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}
	// Build the texture
	if ( !BuildTexture( m_path_source, sourceImage, builtTexture ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}
	// Write the texture to a file
	if ( !WriteTextureToFile( m_path_target, builtTexture ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}

OnExit:
	
	if ( shouldComBeUninitialized )
	{
		CoUninitialize();
	}

	return !wereThereErrors;
}

// Helper Function Definitions
//============================

namespace
{
	bool BuildTexture( const char *const i_path, DirectX::ScratchImage &io_sourceImageThatMayNotBeValidAfterThisCall,
		DirectX::ScratchImage &o_texture )
	{
		// DirectX can only do image processing on uncompressed images
		DirectX::ScratchImage uncompressedImage;
		if ( DirectX::IsCompressed( io_sourceImageThatMayNotBeValidAfterThisCall.GetMetadata().format ) )
		{
			// The uncompressed format is chosen naively and assumes "standard" textures
			// (it will lose precision on any source images that use more than 8 bits per channel
			// and lose information on any that aren't normalized [0,1])
			const DXGI_FORMAT formatToDecompressTo = DXGI_FORMAT_R8G8B8A8_UNORM;
			const HRESULT result = DirectX::Decompress( io_sourceImageThatMayNotBeValidAfterThisCall.GetImages(), io_sourceImageThatMayNotBeValidAfterThisCall.GetImageCount(),
				io_sourceImageThatMayNotBeValidAfterThisCall.GetMetadata(), formatToDecompressTo, uncompressedImage );
			if ( FAILED( result ) )
			{
				eae6320::AssetBuild::OutputErrorMessage( "DirectXTex failed to uncompress source image", i_path );
				return false;
			}
		}
		else
		{
			uncompressedImage = std::move( io_sourceImageThatMayNotBeValidAfterThisCall );
		}
		// Standard DDS images are upside-down from what OpenGL expects
		DirectX::ScratchImage flippedImage;
#if defined ( EAE6320_PLATFORM_GL )
		{
			const DWORD flipVertically = DirectX::TEX_FR_FLIP_VERTICAL;
			const HRESULT result = DirectX::FlipRotate( uncompressedImage.GetImages(), uncompressedImage.GetImageCount(),
				uncompressedImage.GetMetadata(), flipVertically, flippedImage );
			if ( FAILED( result ) )
			{
				eae6320::AssetBuild::OutputErrorMessage( "DirectXTex failed to flip the source image vertically", i_path );
				return false;
			}
		}
#else
		flippedImage = std::move( uncompressedImage );
#endif
		// Direct3D will only load BC compressed textures whose dimensions are multiples of 4
		// ("BC" stands for "block compression", and each block is 4x4)
		DirectX::ScratchImage resizedImage;
		{
			const DirectX::TexMetadata flippedMetadata = flippedImage.GetMetadata();
			size_t targetWidth, targetHeight;
			{
				// Round up to the nearest multiple of 4
				const size_t blockSize = 4;
				targetWidth = eae6320::Math::RoundUpToMultiple_powerOf2(flippedMetadata.width, blockSize);
				targetHeight = eae6320::Math::RoundUpToMultiple_powerOf2(flippedMetadata.height, blockSize);
			}
			if ( ( targetWidth != flippedMetadata.width ) || ( targetHeight != flippedMetadata.height ) )
			{
				const DWORD useDefaultFiltering = DirectX::TEX_FILTER_DEFAULT;
				const HRESULT result = DirectX::Resize( flippedImage.GetImages(), flippedImage.GetImageCount(),
					flippedMetadata, targetWidth, targetHeight, useDefaultFiltering, resizedImage );
			}
			else
			{
				resizedImage = std::move( flippedImage );
			}
		}
		// Generate MIP maps
		// (Note that this will overwrite any existing MIP maps)
		DirectX::ScratchImage imageWithMipMaps;
		{
			const DWORD useDefaultFiltering = DirectX::TEX_FILTER_DEFAULT;
			const size_t generateAllPossibleLevels = 0;
			HRESULT result;
			if ( !resizedImage.GetMetadata().IsVolumemap() )
			{
				result = DirectX::GenerateMipMaps( resizedImage.GetImages(), resizedImage.GetImageCount(),
					resizedImage.GetMetadata(), useDefaultFiltering, generateAllPossibleLevels, imageWithMipMaps );
			}
			else
			{
				result = DirectX::GenerateMipMaps3D( resizedImage.GetImages(), resizedImage.GetImageCount(),
					resizedImage.GetMetadata(), useDefaultFiltering, generateAllPossibleLevels, imageWithMipMaps );
			}
			if ( FAILED( result ) )
			{
				eae6320::AssetBuild::OutputErrorMessage( "DirectXTex failed to generate MIP maps", i_path );
				return false;
			}
		}
		// Compress the texture
		{
			// Our texture builder only supports two kinds of formats:
			//	* BC1 (compressed with no alpha, used to be known as "DXT1")
			//	* BC3 (compressed with alpha, used to be known as "DXT5")
			const DXGI_FORMAT formatToCompressTo = resizedImage.IsAlphaAllOpaque() ? DXGI_FORMAT_BC1_UNORM : DXGI_FORMAT_BC3_UNORM;
			const DWORD useDefaultCompressionOptions = DirectX::TEX_COMPRESS_DEFAULT;
			const float useDefaultThreshold = DirectX::TEX_THRESHOLD_DEFAULT;
			const HRESULT result = DirectX::Compress( imageWithMipMaps.GetImages(), imageWithMipMaps.GetImageCount(),
				imageWithMipMaps.GetMetadata(), formatToCompressTo, useDefaultCompressionOptions, useDefaultThreshold, o_texture );
			if ( FAILED( result ) )
			{
				eae6320::AssetBuild::OutputErrorMessage( "DirectXTex failed to compress the texture", i_path );
				return false;
			}
		}

		return true;
	}

	bool LoadSourceImage( const char *const i_path, DirectX::ScratchImage &o_image )
	{
		// DirectXTex uses wide strings
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> stringConverter;
		const std::wstring path( stringConverter.from_bytes( i_path ) );

		// Open the image based on its file extension
		// (An image's format can also often be deduced by looking at its actual bits
		// because the first of a file will be some kind of recognizable header,
		// but our TextureBuilder keeps things simple)
		HRESULT result = E_UNEXPECTED;
		{
			const std::wstring extension = path.substr( path.find_last_of( L'.' ) + 1 );
			DirectX::TexMetadata* const dontReturnMetadata = NULL;
			if ( extension == L"dds" )
			{
				const DWORD useDefaultBehavior = DirectX::DDS_FLAGS_NONE
					// Just in case you happen to use any old-style DDS files with luminance
					// this will expand the single luminance channel to all RGB channels
					// (which keeps it greyscale rather than using a red-only channel format)
					| DirectX::DDS_FLAGS_EXPAND_LUMINANCE
					;
				result = DirectX::LoadFromDDSFile( path.c_str(), useDefaultBehavior, dontReturnMetadata, o_image );
				if ( FAILED( result ) )
				{
					eae6320::AssetBuild::OutputErrorMessage( "DirectXTex couldn't load the DDS file", i_path );
				}
			}
			else if ( extension == L"tga" )
			{
				result = DirectX::LoadFromTGAFile( path.c_str(), dontReturnMetadata, o_image );
				if ( FAILED( result ) )
				{
					eae6320::AssetBuild::OutputErrorMessage( "DirectXTex couldn't load the TGA file", i_path );
				}
			}
			else
			{
				// Try to Windows Imaging Component and hope it supports the image type
				const DWORD useDefaultBehavior = DirectX::WIC_FLAGS_NONE
					// If an image has an embedded sRGB profile ignore it
					// since our renderer isn't gamma-correct
					// (we want all textures in the shaders to have the same values they do as source images)
					| DirectX::WIC_FLAGS_IGNORE_SRGB
					;
				result = DirectX::LoadFromWICFile( path.c_str(), useDefaultBehavior, dontReturnMetadata, o_image );
				if ( FAILED( result ) )
				{
					eae6320::AssetBuild::OutputErrorMessage( "WIC couldn't load the source image", i_path );
				}
			}
		}

		return SUCCEEDED( result );
	}

	bool WriteTextureToFile( const char *const i_path, const DirectX::ScratchImage &i_texture )
	{
		const DWORD useDefaultBehavior = DirectX::DDS_FLAGS_NONE;
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> stringConverter;
		const HRESULT result = DirectX::SaveToDDSFile( i_texture.GetImages(), i_texture.GetImageCount(), i_texture.GetMetadata(),
			useDefaultBehavior, stringConverter.from_bytes( i_path ).c_str() );
		if ( SUCCEEDED( result ) )
		{
			return true;
		}
		else
		{
			eae6320::AssetBuild::OutputErrorMessage( "DirectXTex failed to save the built texture to disk", i_path );
			return false;
		}
	}
}
