#include "aabb.hpp"

namespace RT_ISICG
{

	bool AABB::intersect( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		Vec3f tMin = VEC3F_ZERO;
		Vec3f tMax = VEC3F_ZERO;

		for (int i = 0; i < 3; ++i) {
			const float t0 = ( _min[ i ] - p_ray.getOrigin()[ i ] ) / p_ray.getDirection()[ i ];
			const float t1 = ( _max[ i ] - p_ray.getOrigin()[ i ] ) / p_ray.getDirection()[ i ];
			tMin[ i ]	   = glm::min( t0, t1 );
			tMax[ i ]	   = glm::max( t0, t1 );
		}

		float tmin = glm::max( glm::max( tMin.x, tMin.y ), tMin.z );
		float tmax = glm::min( glm::min( tMax.x, tMax.y ), tMax.z );

		if ( tmin > tmax ) return false;

		if ( p_tMax < tmin || tmax < p_tMin ) return false;

		return true;
	}

	//Retourne la distance d'intersection avec le AABB
	float AABB::intersectDistance( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		Vec3f t1 = ( _min - p_ray.getOrigin() ) / p_ray.getDirection();
		Vec3f t2 = ( _max - p_ray.getOrigin() ) / p_ray.getDirection();

		Vec3f tMin = glm::min( t1, t2 );
		Vec3f tMax = glm::max( t1, t2 );

		float tNear = std::max( p_tMin, std::max( std::max( tMin.x, tMin.y ), tMin.z ) );
		float tFar	= std::min( p_tMax, std::min( std::min( tMax.x, tMax.y ), tMax.z ) );

		if ( p_tMax < tNear || tFar < p_tMin ) return -1.f;

		if ( tNear <= tFar ) return tNear;
		return -1.f;
	}
} // namespace RT_ISICG
