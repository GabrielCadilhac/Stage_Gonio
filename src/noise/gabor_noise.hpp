#ifndef __RT_ISICG_GABOR_NOISE__
#define __RT_ISICG_GABOR_NOISE__

#include "defines.hpp"

// Gabor noise properties
#define GAUSSIAN_MAGNITUDES 1.f
#define GAUSSIAN_WIDTH 4.f
#define FREQUENCY_MAGNITUDE 8.f
#define FREQUENCY_ORIENTATION 0.f

#define NB_KERNEL 200

namespace RT_ISICG
{
	class GaborNoise
	{
	  public:
		// Generate a full texture of Gabor noise
		static void generate( const int p_width, const int p_height, float * p_output, const int p_seed = 1)
		{
			// Precompute random positions and orientations
			const auto positions = new Vec2f[ NB_KERNEL ];
			const auto orientations = new float[ NB_KERNEL ];
			for (int i = 0; i < NB_KERNEL; i++)
			{
				// Positions random vector 2 between 0 and 1
				positions[ i ]    = Vec2f( randomFloat( p_seed ), randomFloat( p_seed ) );

				//Orientations are between 0 and 2*pi
				orientations[ i ] = randomFloat( p_seed ) * TWO_PIf;
			}

			// min and max are search to normalize noise after
			float minValue = 10.f;
			float maxValue = -10.f;
			for ( int x = 0; x < p_width; ++x )
			{
				for ( int y = 0; y < p_height; ++y )
				{
					// Normalize (x,y) coordinates to (u,v)
					const auto	v = static_cast<float>( y ) / static_cast<float>( p_height );
					const auto	u = static_cast<float>( x ) / static_cast<float>( p_width );

					// Compute noise for all positions and orientations and for one pixel
					const float f = _computeNoise( { u, v }, positions, orientations );

					// Add it to the output
					p_output[ x * p_height + y ] = f;

					// Check min and max
					if ( f < minValue )
						minValue = f;
					else if ( f > maxValue )
						maxValue = f;
				}
			}

			// Normalize value to [0...1]
			for ( int x = 0; x < p_width; ++x )
			{
				for ( int y = 0; y < p_height; ++y )
					p_output[ y * p_width + x ] = ( p_output[ y * p_width + x ] - minValue ) / ( maxValue - minValue );
			}
		}

	  private:

		// Gabor kernel's formula from the paper: Ares Lagae et al. "Procedural Noise Using Sparse Gabor Convolution"
		static float gaussian2D( const Vec2f & p_pos,
								 const float   p_K,
								 const float   p_a,
								 const float   p_F0,
								 const float   p_w0 )
		{
			const float exponent = -PIf * glm::pow( p_a, 2.f ) * ( p_pos.x * p_pos.x + p_pos.y * p_pos.y );
			const float coeff
				= glm::cos( 2.f * PIf * p_F0 * ( p_pos.x * glm::cos( p_w0 ) + p_pos.y * glm::sin( p_w0 ) ) );
			return p_K * coeff * glm::exp( exponent );
		}

		// Compute a noise value for one pixel
		static float _computeNoise( const Vec2f & p_uv,
									 const Vec2f * p_position,
									 const float * p_orientation )
		{
			float sum = 0.f;

			for (int i = 0; i < NB_KERNEL; i++)
				sum += p_orientation[ i ]
					   * gaussian2D( p_uv - p_position[ i ],
									 GAUSSIAN_MAGNITUDES,
									 GAUSSIAN_WIDTH,
									 FREQUENCY_MAGNITUDE,
									 FREQUENCY_ORIENTATION );

			return sum;
		}

	};
} // namespace RT_ISICG

#endif
