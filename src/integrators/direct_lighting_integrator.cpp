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
			Vec3f color = VEC3F_ZERO;

			std::vector<BaseLight *> lights = p_scene.getLights();
			for ( BaseLight * light : lights )
			{
				unsigned int _nbShadowSamples = 1;
				if ( light->getIsSurface() ) _nbShadowSamples = _nbLightSamples;

				for ( unsigned int i = 0; i < _nbShadowSamples; ++i )
				{
					const LightSample lightSample = light->sample( hitRecord._point );
					Ray				  shadowRay( hitRecord._point, lightSample._direction );
					shadowRay.offset( hitRecord._normal );
					if ( !p_scene.intersectAny( shadowRay, 0, lightSample._distance ) )
					{
						const float cosTheta = std::max( glm::dot( hitRecord._normal, lightSample._direction ), 0.f );
						color += _directLighting( hitRecord, lightSample, cosTheta );
					}
				}
			}

			return color / static_cast<float>( _nbLightSamples );
		}
		return _backgroundColor;
	}

	Vec3f DirectLightingIntegrator::_directLighting( const HitRecord &	 p_hitRecord,
													 const LightSample & p_lightSample,
													 const float		 cosTheta ) const
	{
		return p_hitRecord._object->getMaterial()->getFlatColor() * p_lightSample._radiance * cosTheta;
	}
} // namespace RT_ISICG