#ifndef __RT_ISICG_IMAGE__
#define __RT_ISICG_IMAGE__

#include "defines.hpp"
#include <string>
#include <vector>

namespace RT_ISICG
{
	class Texture
	{
	  public:
		Texture() = default;
		Texture( const int p_width, const int p_height, const int p_nbChannels = 3 )
			: _width( p_width ), _height( p_height )
		{
			_pixels = new unsigned char[ static_cast<int>( _width * _height * p_nbChannels ) ];
		}

		Texture( const std::string & p_textureName ) { _load( p_textureName ); }

		virtual ~Texture() = default;

		inline const int			 getWidth() const { return _width; }
		inline const int			 getHeight() const { return _height; }
		inline unsigned char *		 getPixels() { return _pixels; }
		inline const unsigned char * getPixels() const { return _pixels; }
		inline const int			 getNbChannels() const { return _nbChannels; }
		inline void					 setScaleFactor( const float p_newScaleFactor ) { _scaleFactor = p_newScaleFactor; }
		
		virtual Vec2i getPixelCoord( const float p_u, const float p_v, const bool p_isInfinite ) const
		{
			float u = glm::abs(p_u);
			float v = glm::abs(p_v);

			if ( !p_isInfinite )
			{
				u *= static_cast<float>(_height);
				v *= static_cast<float>(_width);
			}

			const int i = static_cast<int>( u * _scaleFactor ) % _width;
			const int j = static_cast<int>( v * _scaleFactor ) % _height;
			return { i, j };
		}

		virtual Vec3f getColorFromPixelCoord( const Vec2i & p_coord ) const
		{
			const int pixelId = (p_coord.x * _width + p_coord.y) * _nbChannels;
			return { _pixels[ pixelId ], _pixels[ pixelId + 1 ], _pixels[ pixelId + 2 ] };
		}

		Vec3f linearFilter( const float p_u, const float p_v, const bool p_isInfinite ) const;
		Vec3f bilinearFilter( const float p_u, const float p_v, const bool p_isInfinite ) const;
		
		void setPixel( const int p_i, const int p_j, const Vec3f & p_color ) const
		{
			assert( _nbChannels == 3 );
			const int pixelId	   = ( p_i + p_j * _width ) * _nbChannels;
			_pixels[ pixelId ]	   = static_cast<unsigned char>( p_color.r * 255 );
			_pixels[ pixelId + 1 ] = static_cast<unsigned char>( p_color.g * 255 );
			_pixels[ pixelId + 2 ] = static_cast<unsigned char>( p_color.b * 255 );
		}

		void setPixel( const int p_i, const int p_j, const Vec4f & p_color ) const
		{
			assert( _nbChannels == 4 );
			const int pixelId	   = ( p_i + p_j * _width ) * _nbChannels;
			_pixels[ pixelId ]	   = static_cast<unsigned char>( p_color.r * 255 );
			_pixels[ pixelId + 1 ] = static_cast<unsigned char>( p_color.g * 255 );
			_pixels[ pixelId + 2 ] = static_cast<unsigned char>( p_color.b * 255 );
			_pixels[ pixelId + 3 ] = static_cast<unsigned char>( p_color.a * 255 );
		}

		void saveJpg( const std::string & p_path, const int p_quality = 100 ) const;

		Vec3f mean() const;

		void addTexture( const Texture & p_texture, const Vec2i & p_offset ) const;

		void mergeTexture( const Texture & p_texture, const bool squared ) const;

	  private:
		void _load( const std::string & p_textureName );

		float _scaleFactor = 1.f;
		int   _nbChannels  = 3;
		int   _width{};
		int   _height{};

		unsigned char * _pixels{};
	};

} // namespace RT_ISICG

#endif // __RT_ISICG_IMAGE__
