#ifndef __RT_ISICG_CONVERT__
#define __RT_ISICG_CONVERT__

#include "defines.hpp"

namespace RT_ISICG
{

	static Vec3f polarToCartesian( const float p_theta, const float p_phi )
	{
		const float x = glm::cos( p_phi ) * glm::sin( p_theta );
		const float y = glm::cos( p_theta );
		const float z = glm::sin( p_phi ) * glm::sin(p_theta);
		return Vec3f( x, y, z );
	}

} // namespace RT_ISICG

#endif // __RT_ISICG_CONVERT__
