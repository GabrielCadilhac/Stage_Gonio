#ifndef __RT_ISICG_WHITTED_INTEGRATOR__
#define __RT_ISICG_WHITTED_INTEGRATOR__

#include "base_integrator.hpp"

namespace RT_ISICG
{
	class WhittedIntegrator : public BaseIntegrator
	{
	  public:
		WhittedIntegrator() : BaseIntegrator() {}
		virtual ~WhittedIntegrator() = default;

		const IntegratorType getType() const override { return IntegratorType::WHITTED; };

		Vec3f Li( const Scene & p_scene, const Ray & p_ray, const float p_tMin, const float p_tMax ) const override;

	  private:
		Vec3f _directLighting( const Scene &	 p_scene,
							   const HitRecord & p_hitRecord,
							   const Ray &		 p_ray ) const;

		Vec3f _trace( const Scene & p_scene,
					  const Ray &	p_ray,
					  const float	p_tMin,
					  const float	p_tMax,
					  const int		p_bounces,
					  const bool p_inObject) const;

		inline const float _f(const Vec3f& p_n, const Vec3f& p_t, const Vec3f& p_o, const float p_nt, const float p_ni) const
		{
			const float num = p_nt * glm::dot( p_n, p_o ) + p_ni * glm::dot( p_n, p_t );
			const float den = p_nt * glm::dot( p_n, p_o ) - p_ni * glm::dot( p_n, p_t );
			return num / den;
		}

		int _nbBounces = 5;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_WHITTED_INTEGRATOR__
