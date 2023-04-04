#ifndef __RT_ISICG_PLASTIC_MATERIAL__
#define __RT_ISICG_PLASTIC_MATERIAL__

#include "base_material.hpp"
#include "brdfs/lambert_brdf.hpp"
#include "brdfs/phong_brdf.hpp"

namespace RT_ISICG
{
	class PlasticMaterial : public BaseMaterial
	{
	  public:
		PlasticMaterial( const std::string & p_name, const Vec3f & p_diffuse, const float p_brillance )
			: BaseMaterial( p_name ), _lambertBRDF( p_diffuse ), _phongBRDF(p_diffuse, p_brillance)
		{
		}

		virtual ~PlasticMaterial() = default;

		Vec3f shade( const Ray &		 p_ray,
					 const HitRecord &	 p_hitRecord,
					 const LightSample & p_lightSample ) const override
		{
			const Vec3f lambertEval = _lambertBRDF.evaluate();
			const Vec3f phongEval	= _phongBRDF.evaluate(p_lightSample._direction, -p_ray.getDirection(), p_hitRecord._normal);
			return phongEval;
		}

		inline const Vec3f & getFlatColor() const override { return _lambertBRDF.getKd(); }

	  protected:
		LambertBRDF _lambertBRDF;
		PhongBRDF	_phongBRDF;
	};

} // namespace RT_ISICG

#endif // __RT_ISICG_PLASTIC_MATERIAL__
