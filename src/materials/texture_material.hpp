#ifndef __RT_ISICG_TEXTURE_MATERIAL__
#define __RT_ISICG_TEXTURE_MATERIAL__

#include "base_material.hpp"
#include "brdfs/cook_torrance_brdf.hpp"
#include "brdfs/micro_brdf.hpp"
#include "brdfs/oren_nayar_brdf.hpp"
#include "brdfs/phong_brdf.hpp"
#include "texture.hpp"

namespace RT_ISICG
{
	class TextureMaterial : public BaseMaterial
	{
	  public:
		TextureMaterial( const std::string & p_name,
						 const std::string & p_textureName,
						 const float		 p_scaleFactor,
						 const float		 p_brillance,
						 const Vec3f &		 p_F0		 = VEC3F_ZERO,
						 const std::string	 p_extension = ".png" )
			: BaseMaterial( p_name ), _scaleFactor( p_scaleFactor ), _brillance( p_brillance ),
			  _microBRDF( p_brillance, p_F0 )
		{
			_F0 = Vec3f( 0.f, 0.f, 0.f );

			_diffuseTexture = new Texture( p_textureName + "Diffuse" + p_extension );
			_normalMap		= new Texture( p_textureName + "Normal" + p_extension );

			_diffuseTexture->setScaleFactor( p_scaleFactor );
			_normalMap->setScaleFactor( p_scaleFactor );
		}

		Vec3f shade( const Ray & p_ray, const HitRecord & p_hit, const LightSample & p_lightSample ) const override
		{
			// And perturbation from normal map
			Vec3f perturbation = _normalMap->bilinearFilter( p_hit._u, p_hit._v, p_hit._object->isPlane() );
			perturbation	   = glm::normalize( perturbation * 2.f - 1.f );

			// Add perturbation to geometric normal by computing a tangent space
			Vec3f newUpVector( 0.f, 0.f, -1.f );
			if ( p_hit._normal.y < -0.99f || p_hit._normal.y > 0.99f ) newUpVector = Vec3f( 1.f, 0.f, 0.f );

			const Vec3f pU = glm::normalize( glm::cross( newUpVector, p_hit._normal ) );
			const Vec3f pV = glm::normalize( glm::cross( pU, p_hit._normal ) );

			Vec3f perturbNormal
				= p_hit._normal + ( pU * perturbation.x + pV * perturbation.y + p_hit._normal * perturbation.z );
			perturbNormal = glm::normalize( perturbNormal );
			perturbNormal = glm::dot( perturbNormal, p_ray.getDirection() ) < 0.f ? perturbNormal : -perturbNormal;

			// Compute dot product
			const float NdotO = glm::dot( p_ray.getDirection(), perturbNormal );
			const float NdotI = glm::dot( p_lightSample._direction, perturbNormal );

			// Evaluate BRDF with perturbated normal and return it
			return _microBRDF.evaluate( p_lightSample._direction, p_ray.getDirection(), perturbNormal );
		}

		inline const Vec3f & getFlatColor() const override { return BLACK; }

		inline const Vec3f & getReflectivity() const override { return VEC3F_ZERO; };

	  protected:
		MicroBRDF _microBRDF;
		Texture * _diffuseTexture = nullptr;
		Texture * _normalMap	  = nullptr;

		float _scaleFactor = 1.f;
		float _brillance   = 0.3f;

		Vec3f _F0;
	};

} // namespace RT_ISICG

#endif // __RT_ISICG_TEXTURE_MATERIAL__
