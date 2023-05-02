#include "virtual_point_light_integrator.hpp"

namespace RT_ISICG
{
	Vec3f VirtualPointLightIntegrator::Li( const Scene & p_scene,
										   const Ray &	 p_ray,
										   const float	 p_tMin,
										   const float	 p_tMax ) const
	{
		HitRecord hitRecord;
		if ( p_scene.intersect( p_ray, p_tMin, p_tMax, hitRecord ) )
		{
			const Vec3f directColor = _directLighting( p_scene, hitRecord, p_ray );
			const Vec3f vplColor	= _VPLLighting( p_scene, hitRecord, p_ray );
			return vplColor;
			//return ( vplColor + directColor ) / 2.f;
		}

		return _backgroundColor;
	}

	Vec3f VirtualPointLightIntegrator::_sampleHemisphere( const Vec3f & p_normal ) const
	{
		const Vec3f randomVect = glm::normalize( Vec3f( randomFloat() * 2.f - 1.f, randomFloat() * 2.f - 1.f, 0.f ) );
		const Vec3f tangent	   = glm::normalize( randomVect - p_normal * glm::dot( randomVect, p_normal ) );
		const Vec3f bitangent  = glm::cross( p_normal, tangent );
		const Mat3f TBN( tangent, bitangent, p_normal );

		const Vec3f sample = glm::normalize(Vec3f( randomFloat() * 2.f - 1.f, randomFloat() * 2.f - 1.f, randomFloat() ));
		return TBN * sample;
	}

	void VirtualPointLightIntegrator::sampleVPL( const Scene & p_scene, const float p_tMax )
	{
		for ( BaseLight * light : p_scene.getLights() )
		{
			if ( !light->getIsSurface() )
			{
				int start = _nbVPL, end = _nbVPL;
				int reflections = 0;

				while ( end > 0 )
				{
					start = static_cast<int>( start * _probaDiffuse );

					for ( int i = start; i < end; ++i )
					{
						Vec3f position
							= light->getPosition() + ( polarToCartesian( halton( 2, i ), halton( 3, i ) ) );

						Vec3f direction
							= glm::normalize( polarToCartesian( halton( 2, i ), halton( 3, i ) ) );

						Ray reflectedRay( position, direction );

						HitRecord hitRecord;
						if ( !p_scene.intersect( reflectedRay, 0.f, p_tMax, hitRecord ) ) continue;

						const float newPower = light->getPower() / glm::pow( hitRecord._distance, 2.f );
						_VPLs.push_back( new PointLight( position, light->getFlatColor(), newPower ) );

						for ( int j = 1; j <= reflections; ++j )
						{
							if ( p_scene.intersect( reflectedRay, 0.f, p_tMax, hitRecord ) )
							{
								const LightSample lightSample = light->sample( hitRecord._point );
								position					  = hitRecord._point + hitRecord._normal;
								direction = _sampleHemisphere( hitRecord._normal );
								reflectedRay		 = Ray( position, direction );
								const float newPower = light->getPower() / glm::pow( hitRecord._distance, 2.f );
								_VPLs.push_back( new PointLight( position, light->getFlatColor(), newPower ) );
							}
							else
								break;
						}
					}

					reflections++;
					end = start;
				}
			}
		}
	}

	Vec3f VirtualPointLightIntegrator::_VPLLighting( const Scene &	   p_scene,
													 const HitRecord & p_hitRecord,
													 const Ray &	   p_ray ) const
	{
		Vec3f		color	  = VEC3F_ZERO;
		const Vec3f flatColor = p_hitRecord._object->getMaterial()->getFlatColor();
		const float N		  = static_cast<float>( _VPLs.size() );
		// Ajouter la couleur L et l'atténuation w dans une classe VPL héritant de PointLight
		for ( PointLight * light : _VPLs )
		{
			const LightSample lightSample = light->sample( p_hitRecord._point );
			Ray				  shadowRay( p_hitRecord._point, lightSample._direction );
			shadowRay.offset( p_hitRecord._normal );

			if ( !p_scene.intersectAny( shadowRay, 0.f, lightSample._distance ) )
			{
				const float cosTheta = std::max( glm::dot( p_hitRecord._normal, lightSample._direction ), 0.f );
				color += ( 1.f / N ) * flatColor * lightSample._radiance * cosTheta;
			}
		}
		return color;
	}
} // namespace RT_ISICG