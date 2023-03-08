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
		{
			Vec3f color = _directLighting( p_scene, hitRecord );

			return color / static_cast<float>( _nbLightSamples );
		}
		return _backgroundColor;
	}

	Vec3f DirectLightingIntegrator::_directLighting( const Scene &	   p_scene,
													 const HitRecord & p_hitRecord ) const
	{
		Vec3f color = VEC3F_ZERO;
		Vec3f colorMaterial = p_hitRecord._object->getMaterial()->getFlatColor();
		for ( BaseLight * light : p_scene.getLights() )
		{
			unsigned int _nbShadowSamples = 1;

			if ( light->getIsSurface() ) _nbShadowSamples = _nbLightSamples;

			for ( unsigned int i = 0; i < _nbShadowSamples; ++i )
			{
				const LightSample lightSample = light->sample( p_hitRecord._point );
				Ray				  shadowRay( p_hitRecord._point, lightSample._direction );
				shadowRay.offset( p_hitRecord._normal );
				if ( !p_scene.intersectAny( shadowRay, 0, lightSample._distance ) )
				{
					const float cosTheta = std::max( glm::dot( p_hitRecord._normal, lightSample._direction ), 0.f );
					color += colorMaterial * lightSample._radiance * cosTheta;
				}
			}
		}
		return color;
	}
} // namespace RT_ISICG