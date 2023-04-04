#include "aabb.hpp"

namespace RT_ISICG
{
	bool AABB::intersect( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		Vec3f t1 = ( _min - p_ray.getOrigin() ) / p_ray.getDirection();
		Vec3f t2 = ( _max - p_ray.getOrigin() ) / p_ray.getDirection();

		Vec3f tMin = glm::min( t1, t2 );
		Vec3f tMax = glm::max( t1, t2 );

		float tNear = std::max(p_tMin, std::max( std::max( tMin.x, tMin.y ), tMin.z ));
		float tFar  = std::min(p_tMax, std::min( std::min( tMax.x, tMax.y ), tMax.z ));

		if ( p_tMax < tNear || tFar < p_tMin ) return false;

		return tNear < tFar;
	}
} // namespace RT_ISICG
