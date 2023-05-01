#include "ray_cast_integrator.hpp"

namespace RT_ISICG
{
	Vec3f RayCastIntegrator::Li( const Scene & p_scene,
								 const Ray &   p_ray,
								 const float   p_tMin,
								 const float   p_tMax ) const
	{
		HitRecord hitRecord;

		if ( !p_scene.intersect( p_ray, p_tMin, p_tMax, hitRecord ) ) return _backgroundColor;

		const float cosTheta = std::max( glm::dot( -p_ray.getDirection(), hitRecord._normal ), 0.f );
		return hitRecord._object->getMaterial()->getFlatColor() * cosTheta;
	}

	void RayCastIntegrator::sampleVPL( const Scene & p_scene, const float p_tMax ) { return; };
} // namespace RT_ISICG
