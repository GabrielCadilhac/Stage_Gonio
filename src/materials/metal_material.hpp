#ifndef __RT_ISICG_METAL_MATERIAL__
#define __RT_ISICG_METAL_MATERIAL__

#include "base_material.hpp"
#include "brdfs/cook_torrance_brdf.hpp"
#include "brdfs/oren_nayar_brdf.hpp"

namespace RT_ISICG
{
	class MetalMaterial : public BaseMaterial
	{
	  public:
		MetalMaterial( const std::string & p_name,
					   const Vec3f &	   p_diffuse,
					   const float		   p_brillance,
					   const float		   p_metalness,
					   const Vec3f &	   p_specular )
			: BaseMaterial( p_name ), _metalness(p_metalness), _orenNayarBRDF( p_diffuse, p_brillance ),
			  _cookTorranceBRDF( p_diffuse,
								 p_brillance,
								 p_specular )
		{
		}

		virtual ~MetalMaterial() = default;

		Vec3f shade( const Ray &		 p_ray,
					 const HitRecord &	 p_hitRecord,
					 const LightSample & p_lightSample ) const override
		{
			const Vec3f diffuse
				= _orenNayarBRDF.evaluate( p_lightSample._direction, p_ray.getDirection(), p_hitRecord._normal );

			const Vec3f specular
				= _cookTorranceBRDF.evaluate( p_lightSample._direction, -p_ray.getDirection(), p_hitRecord._normal );

			return ( 1 - _metalness ) * diffuse + _metalness * specular;
		}

		inline const Vec3f & getFlatColor() const override { return _orenNayarBRDF.getKd(); }

	  protected:
		OrenNayarBRDF	 _orenNayarBRDF;
		CookTorranceBRDF _cookTorranceBRDF;
		float			 _metalness;
	};

} // namespace RT_ISICG

#endif // __RT_ISICG_METAL_MATERIAL__
