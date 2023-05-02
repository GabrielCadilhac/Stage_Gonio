#ifndef __RT_ISICG_CONVERT__
#define __RT_ISICG_CONVERT__

#include "defines.hpp"

namespace RT_ISICG
{

	static Vec3f polarToCartesian( const float p_theta, const float p_phi )
	{
		const float x = glm::cos( TWO_PIf * p_theta ) * 2.f * glm::sqrt( p_phi * ( 1.f - p_phi ) );
		const float y = glm::sin( TWO_PIf * p_theta ) * 2.f * glm::sqrt( p_phi * ( 1.f - p_phi ) );
		const float z = 1.f - 2.f * p_phi;
		return Vec3f( x, y, z );
	}

} // namespace RT_ISICG

#endif // __RT_ISICG_CONVERT__
