#ifndef __RT_ISICG_RANDOM__
#define __RT_ISICG_RANDOM__

#include <random>

namespace RT_ISICG
{
	// Return a pseudo random float between 0 and 1
	static inline float randomFloat()
	{
		static std::mt19937							 gen;
		static std::uniform_real_distribution<float> dis( 0.f, 1.f );
		return dis( gen );
	}

	// Return a Halton sequence based number between 0 and 1
	static inline double halton( const int p_base, int p_index )
	{
		double x = 0.0, f = 1.0 / p_base;

		while ( p_index )
		{
			x += f * static_cast<double>( p_index % p_base );
			p_index /= p_base;
			f *= ( 1.0 / static_cast<double>( p_base ) );
		}

		return x;
	}

} // namespace RT_ISICG

#endif // __RT_ISICG_RANDOM__
