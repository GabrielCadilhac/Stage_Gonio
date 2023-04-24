#ifndef __RT_ISICG_WHITTED_INTEGRATOR__
#define __RT_ISICG_WHITTED_INTEGRATOR__

#include "direct_lighting_integrator.hpp"

namespace RT_ISICG
{
	class WhittedIntegrator : public DirectLightingIntegrator
	{
	  public:
		WhittedIntegrator() : DirectLightingIntegrator() {}
		virtual ~WhittedIntegrator() = default;

		const IntegratorType getType() const override { return IntegratorType::WHITTED; };

		Vec3f Li( const Scene & p_scene, const Ray & p_ray, const float p_tMin, const float p_tMax ) const override;

	  private:

		Vec3f _trace( const Scene & p_scene,
					  const Ray &	p_ray,
					  const float	p_tMin,
					  const float	p_tMax,
					  const int		p_bounces,
					  const bool p_inObject) const;

		int _nbBounces = 5;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_WHITTED_INTEGRATOR__
