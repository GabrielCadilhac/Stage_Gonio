#ifndef __RT_ISICG_CONVERT__
#define __RT_ISICG_CONVERT__

#include "defines.hpp"

namespace RT_ISICG
{

	static Vec3f polarToCartesian( const float p_theta, const float p_phi )
	{
		const float x = glm::sin( p_phi ) * glm::cos( p_theta );
		const float y = glm::cos( p_phi );
		const float z = glm::sin( p_phi ) * glm::sin( p_theta );
		return { x, y, z };
	}

	static std::string vec3ToString( const Vec3f & p_vector )
	{
		std::string stringVec;
		stringVec += std::to_string( p_vector.x ) + ",";
		stringVec += std::to_string( p_vector.y ) + ",";
		stringVec += std::to_string( p_vector.z );
		return stringVec;
	}

	static Vec3f squareVector( const Vec3f & p_vec )
	{
		return Vec3f( p_vec.x * p_vec.x, p_vec.y * p_vec.y, p_vec.z * p_vec.z );
	}
} // namespace RT_ISICG

#endif // __RT_ISICG_CONVERT__
