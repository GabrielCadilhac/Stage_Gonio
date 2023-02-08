#include "plane_geometry.hpp"

namespace RT_ISICG
{
	bool PlaneGeometry::intersect(const Ray& p_ray, float& p_t) const {
		float denominateur = glm::dot( _normal, p_ray.getDirection() );

		if (denominateur != 0) {
			Vec3f po = _point - p_ray.getOrigin();
			p_t		 = glm::dot( po, _normal ) / denominateur;
			return true;
		}
		return false;
	}
}