#include "cTextureBuilder.h"
#include <codecvt>
#include <locale>
#include <string>
#include <utility>
#include "../../Engine/Math/Functions.h"
#include "../../External/DirectXTex/Includes.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"

namespace
{
	bool BuildTexture( const char *const i_path, DirectX::ScratchImage &io_sourceImageThatMayNotBeValidAfterThisCall,
		DirectX::ScratchImage &o_texture );
	bool LoadSourceImage( const char *const i_path, DirectX::ScratchImage &o_image );
	bool WriteTextureToFile( const char *const i_path, const DirectX::ScratchImage &i_texture );
}

bool eae6320::AssetBuild::cTextureBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;

	DirectX::ScratchImage sourceImage;
	DirectX::ScratchImage builtTexture;
	bool shouldComBeUninitialized = false;

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
	
	if ( !LoadSourceImage( m_path_source, sourceImage ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}
	
	if ( !BuildTexture( m_path_source, sourceImage, builtTexture ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}
	
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

namespace
{
	bool BuildTexture( const char *const i_path, DirectX::ScratchImage &io_sourceImageThatMayNotBeValidAfterThisCall,
		DirectX::ScratchImage &o_texture )
	{
		DirectX::ScratchImage uncompressedImage;
		if ( DirectX::IsCompressed( io_sourceImageThatMayNotBeValidAfterThisCall.GetMetadata().format ) )
		{
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
		DirectX::ScratchImage resizedImage;
		{
			const DirectX::TexMetadata flippedMetadata = flippedImage.GetMetadata();
			size_t targetWidth, targetHeight;
			{
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
		
		{
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
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> stringConverter;
		const std::wstring path( stringConverter.from_bytes( i_path ) );

		HRESULT result = E_UNEXPECTED;
		{
			const std::wstring extension = path.substr( path.find_last_of( L'.' ) + 1 );
			DirectX::TexMetadata* const dontReturnMetadata = NULL;
			if ( extension == L"dds" )
			{
				const DWORD useDefaultBehavior = DirectX::DDS_FLAGS_NONE
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
				const DWORD useDefaultBehavior = DirectX::WIC_FLAGS_NONE
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
