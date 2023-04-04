#include "whitted_integrator.hpp"

namespace RT_ISICG
{
	Vec3f WhittedIntegrator::Li( const Scene & p_scene,
								 const Ray &   p_ray,
								 const float   p_tMin,
								 const float   p_tMax ) const
	{
		HitRecord hitRecord;
		if ( p_scene.intersect( p_ray, p_tMin, p_tMax, hitRecord ) )
			return _directLighting( p_scene, hitRecord, p_ray );

		return _backgroundColor;
	}

	Vec3f WhittedIntegrator::_trace( const Scene &		 p_scene,
									 const Ray &		 p_ray,
									 const HitRecord &	 p_hitRecord,
									 const LightSample & p_lightSample,
									 const int			 p_bounces )
	{
		if ( p_bounces >= _nbBounces ) return BLACK;

		if ( p_hitRecord._object->getMaterial()->isMirror() )
		{
			const Vec3f reflection = glm::reflect( p_lightSample._direction, p_hitRecord._normal );
			const Ray	rayonReflexion( p_hitRecord._point, reflection );
			_trace( p_scene, rayonReflexion, p_hitRecord, p_lightSample );
		}
		else { return _directLighting( p_scene, p_hitRecord, p_ray ); }
	}

	Vec3f WhittedIntegrator::_directLighting( const Scene &		p_scene,
											  const HitRecord & p_hitRecord,
											  const Ray &		p_ray ) const
	{
		Vec3f color = VEC3F_ZERO;
		for ( BaseLight * light : p_scene.getLights() )
		{
			Vec3f tempColor		   = VEC3F_ZERO;
			float _nbShadowSamples = 1;

			if ( light->getIsSurface() ) _nbShadowSamples = static_cast<float>( _nbLightSamples );

			for ( unsigned int i = 0; i < _nbShadowSamples; ++i )
			{
				const LightSample lightSample = light->sample( p_hitRecord._point );
				Ray				  shadowRay( p_hitRecord._point, lightSample._direction );
				shadowRay.offset( p_hitRecord._normal );

				if ( !p_scene.intersectAny( shadowRay, 0, lightSample._distance ) )
				{
					Vec3f colorMaterial	 = p_hitRecord._object->getMaterial()->shade( p_ray, p_hitRecord, lightSample );
					const float cosTheta = std::max( glm::dot( p_hitRecord._normal, lightSample._direction ), 0.f );
					tempColor += colorMaterial * lightSample._radiance * cosTheta;
				}
			}
			color += ( tempColor / _nbShadowSamples );
		}
		return color / static_cast<float>( p_scene.getLights().size() );
	}
} // namespace RT_ISICG