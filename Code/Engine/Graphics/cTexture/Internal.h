#ifndef GRAPHICS_CTEXTURE_INTERNAL_H
#define GRAPHICS_CTEXTURE_INTERNAL_H

namespace Engine
{
	namespace Graphics
	{
		namespace TextureFormats
		{
			enum eFormat
			{
				BC1,
				BC2,
				BC3,
				BC4,
				BC5,
				BC6h,
				BC7,

				Unknown
			};
			inline unsigned int GetSizeOfBlock( const eFormat i_format )
			{
				switch ( i_format )
				{
					case BC1: return 8; break;
					case BC2: return 8 + ( 4 * 16 );
					case BC3: return 8 + 8; break;
					case BC4: return 8; break;
					case BC5: return 8 + 8; break;
					case BC6h: return 16; break;
					case BC7: return 16; break;
				}
				return 0;
			}
		}

		struct sDdsInfo
		{
			const void* imageData;
			size_t imageDataSize;
			unsigned int width, height;
			unsigned int mipLevelCount;
			TextureFormats::eFormat format;

			sDdsInfo()
				:
				imageData( nullptr ), imageDataSize( 0 ),
				width( 0 ), height( 0 ), mipLevelCount( 0 ),
				format( TextureFormats::Unknown )
			{

			}
		};
	}
}

#endif
