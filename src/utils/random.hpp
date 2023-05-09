#ifndef __RT_ISICG_RANDOM__
#define __RT_ISICG_RANDOM__

#include <random>
#include "utils/convert.hpp"

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
	static inline float halton( const int p_base, int p_index )
	{
		float x = 0.f, f = 1.f / static_cast<float>(p_base);

		while ( p_index )
		{
			x += f * static_cast<float>( p_index % p_base );
			p_index /= p_base;
			f *= ( 1.f / static_cast<float>( p_base ) );
		}

		return x;
	}

	static inline Vec3f sampleHemisphere( const Vec3f & p_normal, const float theta, const float phi )
	{
		const Vec3f direction = polarToCartesian( theta, phi );
		return glm::dot( direction, glm::normalize( p_normal ) ) < 0.f ? direction : -direction;
	}

} // namespace RT_ISICG

#endif // __RT_ISICG_RANDOM__
