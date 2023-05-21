#ifndef __RT_ISICG_VIRTUAL_POINT_LIGHT_INTEGRATOR__
#define __RT_ISICG_VIRTUAL_POINT_LIGHT_INTEGRATOR__

#include "direct_lighting_integrator.hpp"
#include "light_tree.hpp"
#include "lights/point_light.hpp"
#include "utils/convert.hpp"
#include "utils/random.hpp"
#include <vector>

namespace RT_ISICG
{
	class VirtualPointLightIntegrator : public DirectLightingIntegrator
	{
	  public:
		VirtualPointLightIntegrator() : DirectLightingIntegrator() {}
		virtual ~VirtualPointLightIntegrator() = default;

		const IntegratorType getType() const override { return IntegratorType::VIRTUAL_POINT_LIGHT; }

		Vec3f Li( const Scene & p_scene, const Ray & p_ray, const float p_tMin, const float p_tMax ) const override;

		void		sampleVPL( const Scene & p_scene, const float p_tMin, const float p_tMax );
		inline void addHitRecordSample( const HitRecord & p_hitRecord ) { _hitRecordSamples.push_back( p_hitRecord ); };

		void lightCutsLighting( const Scene & p_scene, const HitRecord & p_hitRecord, const Ray & p_ray ) const;

	  private:
		Vec3f _VPLLighting( const Scene & p_scene, const HitRecord & p_hitRecord, const Ray & p_ray ) const;

		float _computeAcceptanceProbability( const PointLight * p_light, const float p_totalLuminance ) const;
		float _totalLuminance( const Scene & p_scene, const float p_tMin, const float p_tMax ) const;

		const int	_nbVPL				= 100;
		const float _probaDiffuse		= 0.2f;
		const int	_nbHitRecordSamples = 100;
		const float _epsilon			= 0.05f;

		std::vector<PointLight *> _VPLs;
		std::vector<HitRecord>  _hitRecordSamples;
		LightTree				  _lightTree;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_VIRTUAL_POINT_LIGHT_INTEGRATOR__
