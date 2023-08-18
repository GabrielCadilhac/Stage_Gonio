#ifndef __RT_ISICG_PERLIN_NOISE__
#define __RT_ISICG_PERLIN_NOISE__

#include "defines.hpp"
#include "utils/random.hpp"

#define NB_OCTAVES 4
#define BIAS 2.f

namespace RT_ISICG
{
	class PerlinNoise
	{
	public:
		// Generate Perlin Noise
		static inline void generate( const int   p_width,
								     const int   p_height,
								     float *	 p_output,
								     const int   p_seed = 1 )
		{
			constexpr Vec2i resolution { 128 };
			const auto		randomGrid = new float *[ resolution.y + 1 ];

			_genRandomGrid( resolution, randomGrid, p_seed );

			for ( int x = 0; x < p_width; ++x )
			{
				for ( int y = 0; y < p_height; ++y )
				{
					// Normalize coordinates between 0 and 1
					const float tX = static_cast<float>( x ) / static_cast<float>( p_width );
					const float tY = static_cast<float>( y ) / static_cast<float>( p_height );

					Vec2f uv { tX, tY };
					uv *= 8.f;

					float f		= 0.f;
					float scale = 0.5f;

					for ( int octave = 0; octave < NB_OCTAVES; octave++ )
					{
						f += scale
							 * _noise( { glm::mod( uv.x, static_cast<float>( resolution.x ) ),
										 glm::mod( uv.y, static_cast<float>( resolution.y ) ) },
									   randomGrid );
						uv = 2.f * uv;
						scale /= BIAS;
					}

					p_output[ y * p_width + x ] = f;
				}
			}
		}

	  private:
		static float _noise( const Vec2f & p_p, float ** p_randomGrid )
		{
			const auto	i = Vec2i( floor( p_p ) );
			const Vec2f f = glm::fract( p_p );

			const Vec2f u = f * f * ( 3.f - 2.f * f );
			return glm::mix( glm::mix( p_randomGrid[ i.x ][ i.y ], p_randomGrid[ i.x + 1 ][ i.y ], u.x ),
							 glm::mix( p_randomGrid[ i.x ][ i.y + 1 ], p_randomGrid[ i.x + 1 ][ i.y + 1 ], u.x ),
							 u.y );
		}

		static void _genRandomGrid( const Vec2i p_resolution, float ** p_grid, const int p_seed = 1 )
		{
			for ( int i = 0; i < p_resolution.y + 1; i++ )
			{
				p_grid[ i ] = new float[ p_resolution.x + 1 ];
				for ( int j = 0; j < p_resolution.x + 1; j++ )
					p_grid[ i ][ j ] = randomFloat( p_seed );
			}
		}
	};

}

#endif