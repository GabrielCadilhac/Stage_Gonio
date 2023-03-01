#include "plane_geometry.hpp"

namespace RT_ISICG
{
	bool PlaneGeometry::intersect(const Ray& p_ray, float& p_t) const {
		const float denominateur = glm::dot( glm::normalize( p_ray.getDirection() ), glm::normalize( _normal ) );
		p_t						 = -1.f;

		if (denominateur != 0) {
			const Vec3f po = _point - p_ray.getOrigin();
			p_t		 = glm::dot( po, _normal ) / denominateur;
			return true;
		}
		return false;
	}
}