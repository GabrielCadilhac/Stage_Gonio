#include "sphere_geometry.hpp"

namespace RT_ISICG
{
	bool SphereGeometry::intersect( const Ray & p_ray, float & p_t1, float & p_t2 ) const
	{
		Vec3f oc = p_ray.getOrigin() - _center;
		float b	= 2.f * glm::dot( oc, p_ray.getDirection() );
		float c	 = glm::dot( oc, oc ) - _radius * _radius;
		float delta = b * b - 4 * c;

		p_t1 = -1.f;
		p_t2 = -1.f;

		if (delta >= 0)
		{
			p_t1 = ( -b - glm::sqrt( delta ) ) / 2.f;
			if (p_t1 > 0)
				p_t2 = ( -b + glm::sqrt( delta ) ) / 2.f;
			
			return true;
		}
		return false;
	}

} // namespace RT_ISICG
