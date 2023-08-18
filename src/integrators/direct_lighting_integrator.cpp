#include "direct_lighting_integrator.hpp"

namespace RT_ISICG
{
	Vec3f DirectLightingIntegrator::Li( const Scene & p_scene,
										const Ray &	  p_ray,
										const float	  p_tMin,
										const float	  p_tMax ) const
	{
		HitRecord hitRecord;
		if ( p_scene.intersect( p_ray, p_tMin, p_tMax, hitRecord ) )
			return _directLighting( p_scene, hitRecord, p_ray );

		return _backgroundColor;
	}

	Vec3f DirectLightingIntegrator::_directLighting( const Scene &	   p_scene,
													 const HitRecord & p_hitRecord,
													 const Ray &	   p_ray ) const
	{
		Vec3f color = VEC3F_ZERO;
		for ( BaseLight * light : p_scene.getLights() )
		{
			Vec3f tempColor		  = VEC3F_ZERO;
			float nbShadowSamples = 1;

			if ( light->getIsSurface() )
				nbShadowSamples = static_cast<float>( _nbLightSamples );

			for ( unsigned int i = 0; i < static_cast<unsigned int>(nbShadowSamples); ++i )
			{
				const LightSample lightSample = light->sample( p_hitRecord._point );
				Ray				  shadowRay( p_hitRecord._point, lightSample._direction );
				shadowRay.offset( p_hitRecord._normal );

				if ( !p_scene.intersectAny( shadowRay, 0.f, FLT_MAX ) )
				{
					Vec3f colorMaterial	 = p_hitRecord._object->getMaterial()->shade( p_ray, p_hitRecord, lightSample );
					const float cosTheta = std::max( glm::dot( p_hitRecord._normal, lightSample._direction ), 0.f );
					tempColor += colorMaterial * lightSample._radiance * cosTheta;
				}
			}
			color += ( tempColor / nbShadowSamples );
		}
		return color / static_cast<float>( p_scene.getLights().size() );
	}
} // namespace RT_ISICG