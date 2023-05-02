#ifndef __RT_ISICG_VIRTUAL_POINT_LIGHT_INTEGRATOR__
#define __RT_ISICG_VIRTUAL_POINT_LIGHT_INTEGRATOR__

#include "direct_lighting_integrator.hpp"
#include "lights/point_light.hpp"
#include "utils/random.hpp"
#include <vector>
#include "utils/convert.hpp"

namespace RT_ISICG
{
	class VirtualPointLightIntegrator : public DirectLightingIntegrator
	{
	  public:
		VirtualPointLightIntegrator() : DirectLightingIntegrator() {}
		virtual ~VirtualPointLightIntegrator() = default;

		const IntegratorType getType() const override { return IntegratorType::VIRTUAL_POINT_LIGHT; };

		Vec3f Li( const Scene & p_scene, const Ray & p_ray, const float p_tMin, const float p_tMax ) const override;

		void sampleVPL( const Scene & p_scene, const float p_tMax ) override;

	  private:
		Vec3f _VPLLighting( const Scene & p_scene, const HitRecord & p_hitRecord, const Ray & p_ray ) const;

		Vec3f _sampleHemisphere( const Vec3f & p_normal ) const;

		const int				  _nbVPL = 90;
		const float				  _probaDiffuse = 0.1f;
		std::vector<PointLight *> _VPLs;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_VIRTUAL_POINT_LIGHT_INTEGRATOR__
