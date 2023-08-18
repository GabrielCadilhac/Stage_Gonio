#ifndef __RT_ISICG_PERLIN_NOISE_HPP__
#define __RT_ISICG_PERLIN_NOISE_HPP__

#include "defines.hpp"

// other noise
#include "noise/perlin_noise.hpp"
#include "noise/lrpn.hpp"
#include "noise/gabor_noise.hpp"

namespace RT_ISICG
{
	class Noise
	{
	  public:
		Noise()	 = default;
		~Noise() = default;

		static inline void perlinNoise( const int	p_width,
										const int	p_height,
										float *		p_output,
										const int	p_seed = 1 )
		{
			PerlinNoise::generate( p_width, p_height, p_output, p_seed );
		}

		static inline void lrpn( const int p_width, const int p_height, float * p_output, const int p_seed = 1 )
		{
			LRPN::generateNoise( p_width, p_height, p_output, p_seed );
		}

		static inline void gaborNoise(const int p_width, const int p_height, float * p_output, const int p_seed )
		{
			GaborNoise::generate( p_width, p_height, p_output, p_seed );
		}

		static inline void convert2Normal( const Vec2i	 p_resolution,
										   const float * p_noise,
										   Vec3f *		 p_output,
										   const float	 p_scale = 4.f)
		{
			for ( int y = 0; y < p_resolution.y; ++y )
			{
				for ( int x = 0; x < p_resolution.x; ++x )
					p_output[ x + y * p_resolution.x ] = _sobelFilter( x, y, p_noise, p_scale, p_resolution );
			}
		}

	  private:
		static inline Vec3f _sobelFilter( const int	  p_x,
												const int	  p_y,
												const float * p_noise,
												const float	  p_scale,
												const Vec2i & p_resolution )
		{
			float intensities[ 9 ];

			// Compute convolution filter
			for ( int i = 0; i < 3; i++ )
			{
				for ( int j = 0; j < 3; j++ )
				{
					const int newI			 = glm::clamp( p_x + i - 1, 0, p_resolution.x );
					const int newJ			 = glm::clamp( p_y + j - 1, 0, p_resolution.y );
					intensities[ i * 3 + j ] = _getHeight( p_noise, Vec2i( newI, newJ ), p_resolution );
				}
			}

			const float dX = ( intensities[ 2 ] + 2.f * intensities[ 5 ] + intensities[ 8 ] )
							 - ( intensities[ 0 ] + 2.f * intensities[ 3 ] + intensities[ 6 ] );
			const float dY = ( intensities[ 6 ] + 2.f * intensities[ 7 ] + intensities[ 8 ] )
							 - ( intensities[ 0 ] + 2.f * intensities[ 1 ] + intensities[ 2 ] );
			const float dZ = 1.f / p_scale;

			return glm::normalize( Vec3f( dX, dY, dZ ) ) * 0.5f + 0.5f;
		}

		static inline float _getHeight( const float * p_noise, const Vec2i & p_uv, const Vec2i & p_resolution )
		{
			return p_noise[ p_uv.y * p_resolution.x + p_uv.x ];
		}
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_PERLIN_NOISE_HPP__
