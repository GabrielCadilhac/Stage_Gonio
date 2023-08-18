#include "texture.hpp"
#include "io/stb_include.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb/stb_image.h"

#include "utils/convert.hpp"
namespace RT_ISICG
{
	void Texture::saveJpg( const std::string & p_path, const int p_quality ) const
	{
		stbi_write_jpg( p_path.c_str(), static_cast<int>(_width), _height, _nbChannels, _pixels, p_quality );
	}

	void Texture::_load( const std::string & p_textureName )
	{
		_pixels = stbi_load( p_textureName.c_str(), &_width, &_height, &_nbChannels, 0 );
	}

	Vec3f Texture::linearFilter( const float p_u, const float p_v, const bool p_isInfinite ) const
	{
		const Vec2i pixel = getPixelCoord( p_u, p_v, p_isInfinite );
		return getColorFromPixelCoord( pixel ) / 255.f;
	}

	Vec3f Texture::bilinearFilter( const float p_u, const float p_v, const bool p_isInfinite ) const
	{
		const Vec2i pixel00 = getPixelCoord( p_u, p_v, p_isInfinite );
		const Vec2i pixel01( pixel00.x, pixel00.y + 1 );
		const Vec2i pixel10( pixel00.x + 1, pixel00.y );
		const Vec2i pixel11( pixel00.x + 1, pixel00.y + 1 );

		Vec3f texelColor = getColorFromPixelCoord( pixel00 );
		float nbTexel	 = 1.f;

		if (pixel10.x < _width)
		{
			texelColor += getColorFromPixelCoord( pixel10 );
			nbTexel++;
		}

		if (pixel01.y < _height)
		{
			texelColor += getColorFromPixelCoord( pixel01 );
			nbTexel++;
		}

		if (pixel11.x < _width && pixel11.y < _height)
		{
			texelColor += getColorFromPixelCoord( pixel11 );
			nbTexel++;
		}

		return texelColor / ( nbTexel * 255.f );
	}

	Vec3f Texture::mean() const
	{
		Vec3f somme = VEC3F_ZERO;
		for (int j = 0; j < _height; ++j) {
			for ( int i = 0; i < _width; ++i )
				somme += getColorFromPixelCoord( Vec2i( j, i ) );
		}
		somme /= _width * _height;
		return somme;
	}

	void Texture::addTexture(const Texture & p_texture, const Vec2i & p_offset) const
	{
		for ( int i = 0; i < p_texture.getHeight(); ++i )
		{
			for (int j = 0; j < p_texture.getWidth(); ++j)
			{
				const Vec3f color = p_texture.getColorFromPixelCoord( { i, j } ) / 255.f;
				setPixel( j + p_offset.x, i + p_offset.y, color );
			}
		}
	}

	void Texture::mergeTexture( const Texture & p_texture, const bool squared = false ) const
	{
		for ( int i = 0; i < p_texture.getHeight(); ++i )
		{
			for ( int j = 0; j < p_texture.getWidth(); ++j )
			{
				Vec3f color1 = p_texture.getColorFromPixelCoord( { i, j } );
				color1		 = Vec3f( ( color1.x + color1.y + color1.z ) / 3.f );

				if ( squared )
					color1 = squareVector( color1 );

				Vec3f color2 = getColorFromPixelCoord( { i, j } );

				setPixel( j, i, (color1 + color2) / 255.f);
			}
		}
	}

} // namespace RT_ISICG
