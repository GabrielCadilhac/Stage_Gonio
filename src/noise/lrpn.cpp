#include "lrpn.hpp"
#include "utils/random.hpp"

namespace RT_ISICG
{
	void LRPN::generateNoise( const int p_width, const int p_height, float * p_output, const int p_seed )
	{
		constexpr int I = 9;

		// Precompute noises
		const auto freqs	  = new Vec2f *[ I ];
		const auto amplitudes = new float *[ I ];
		const auto phase	  = new float *[ I ];
		const auto positions  = new Vec2f[ I ];

		for ( int i = 0; i < I; i++ )
		{
			freqs[ i ]		= new Vec2f[ NB_OF_COSINES ];
			amplitudes[ i ] = new float[ NB_OF_COSINES ];
			phase[ i ]		= new float[ NB_OF_COSINES ];
			positions[ i ]	= Vec2f( randomFloat( p_seed ), randomFloat( p_seed ) ) * 2.f - 1.f;

			for ( int j = 0; j < NB_OF_COSINES; j++ )
			{
				const float freqX = randomFloat( p_seed );
				const float freqY = randomFloat( p_seed );
				freqs[ i ][ j ]	  = Vec2f( freqX, freqY );

				amplitudes[ i ][ j ] = randomFloat( p_seed ) * 2.f - 1.f;
				phase[ i ][ j ]		 = randomFloat( p_seed ) * 2.f - 1.f;
			}
		}

		// Compute LRPN array
		for ( int x = 0; x < p_width; ++x )
		{
			for ( int y = 0; y < p_height; ++y )
			{
				// Normalize coordinates between 0 and 1
				const float tX = static_cast<float>( x ) / static_cast<float>( p_width );
				const float tY = static_cast<float>( y ) / static_cast<float>( p_height );

				// Compute noise for each pixels
				const Vec3f lrpn = _noiseLRPN( { tX, tY }, freqs, amplitudes, phase, positions );

				// Save in the output
				p_output[ y * p_width + x ] = glm::clamp( 0.5f - 0.5f * lrpn.x, 0.f, 1.f );
			}
		}

		// Clean all precomputed noises
		for ( int i = 0; i < I; i++ )
		{
			delete[] freqs[ i ];
			delete[] amplitudes[ i ];
			delete[] phase[ i ];
		}

		delete[] freqs;
		delete[] amplitudes;
		delete[] phase;
		delete[] positions;
	}

	Vec3f LRPN::_noiseLRPN( const Vec2f & p_texCoords,
							Vec2f **	  p_freqs,
							float **	  p_amplitudes,
							float **	  p_phase,
							const Vec2f * p_positions )
	{
		// Noise properties
		constexpr Vec2f rangeFrequency { 0.3f, 0.5f };
		constexpr Vec2f rangeOrientation { 0.f, 1.f };

		// Noise pixel constants
		float			lrpn   = 0.f;
		constexpr float weight = 1.f / static_cast<float>( NB_OF_COSINES );
		const Mat2f		sigma( ( 1.33f * 1.33f ) / TWO_PIf );

		// 8 neighbors with near random positions
		for ( int i = 0; i < 9; i++ )
		{
			float sumOfCosines = 0.f;

			const Vec2f xy = p_texCoords - p_positions[ i ] - Vec2f( .5f );

			// Compute all cosines
			for ( int k = 0; k < NB_OF_COSINES; k++ )
			{
				const float freq = ( p_freqs[ i ][ k ].x * ( rangeFrequency.y - rangeFrequency.x ) + rangeFrequency.x )
								   * LRPN_RESOLUTION;
				const float orientation
					= p_freqs[ i ][ k ].y * ( rangeOrientation.y - rangeOrientation.x ) + rangeOrientation.x;
				const float phase = p_phase[ i ][ k ];

				// Papers formula
				const float ff
					= TWO_PIf * glm::dot( xy, freq * Vec2f( glm::cos( orientation ), glm::sin( orientation ) ) );

				sumOfCosines += p_amplitudes[ i ][ k ] * glm::cos( ff + phase );
			}

			lrpn += _gaussian( xy, 1.f, VEC2F_ZERO, sigma ) * weight * sumOfCosines;
		}

		// Return noise for one pixels
		return Vec3f( lrpn );
	}
} // namespace RT_ISICG
