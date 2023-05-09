#ifndef __RT_ISICG_DIRECT_LIGHTING_INTEGRATOR__
#define __RT_ISICG_DIRECT_LIGHTING_INTEGRATOR__

#include "base_integrator.hpp"

namespace RT_ISICG
{
	class DirectLightingIntegrator : public BaseIntegrator
	{
	  public:
		DirectLightingIntegrator() : BaseIntegrator() {}
		virtual ~DirectLightingIntegrator() = default;

		const IntegratorType getType() const override { return IntegratorType::DIRECT_LIGHTING; }

		Vec3f Li( const Scene & p_scene, const Ray & p_ray, const float p_tMin, const float p_tMax ) const override;

	  protected:
		Vec3f _directLighting( const Scene & p_scene, const HitRecord & p_hitRecord, const Ray & p_ray ) const;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_DIRECT_LIGHTING_INTEGRATOR__
