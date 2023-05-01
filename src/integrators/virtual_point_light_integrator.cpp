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
			Vec3f directColor = _directLighting( p_scene, hitRecord, p_ray );
			Vec3f vplColor	  = _VPLLighting( p_scene, hitRecord, p_ray );
			return 0.7f * directColor + 0.3f * vplColor;
		}

		return _backgroundColor;
	}

	void VirtualPointLightIntegrator::_computeVPL( const Scene &	 p_scene,
												   const float		 p_tMax,
												   const Vec3f &	 p_position,
												   const BaseLight * p_light )
	{
		const Vec3f pointOnSphere = p_position + p_light->getPosition();
		Ray			lightRay( pointOnSphere, glm::normalize( p_position ) );

		HitRecord hitRecord;
		if ( p_scene.intersect( lightRay, 0.f, p_tMax, hitRecord ) )
		{
			const LightSample lightSample = p_light->sample( hitRecord._point );
			const float		  radiance	  = p_light->getPower();
			_VPLs.push_back(
				new PointLight( hitRecord._point + hitRecord._normal, p_light->getFlatColor(), radiance ) );

			const Vec3f reflectedDir = glm::reflect( lightRay.getDirection(), hitRecord._normal );
			lightRay				 = Ray( hitRecord._point, reflectedDir );
		}
	}

	void VirtualPointLightIntegrator::sampleVPL( const Scene & p_scene, const float p_tMax )
	{
		for ( BaseLight * light : p_scene.getLights() )
		{
			if ( !light->getIsSurface() )
			{
				int	  start = _nbVPL, end = _nbVPL;
				int	  reflections = 0;

				while ( end > 0 )
				{
					start *= _probaAbsorbed;

					for ( int i = start; i < end; ++i )
					{
						const Vec2f y( halton( 2, i ), halton( 3, i ) );

						const float x = glm::cos( TWO_PIf * y.x ) * 2.f * glm::sqrt( y.y * ( 1.f - y.y ) );
						const float y = glm::sin( TWO_PIf * y.x ) * 2.f * glm::sqrt( y.y * ( 1.f - y.y ) );
						const float z = 1.f - 2.f * y.y;

						Vec3f position( x, y, z );

						for ( int j = 0; j <= reflections; ++j )
						{
							_computeVPL( p_scene, p_tMax, position, light );

							const Vec2f dir( glm::asin( glm::sqrt( halton( 2 * j + 2, i ) ) ),
											 TWO_PIf * halton( 2 * j + 3, i ) );

							Ray			reflectedRay( position, glm::normalize( dir ) );

							HitRecord hitRecord;
							if ( p_scene.intersect( reflectedRay, 0.f, p_tMax, hitRecord ) )
								position = hitRecord._point;
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
		const Vec3f flatColor = p_hitRecord._object->getMaterial()->getFlatColor() / PIf;
		const float N		  = static_cast<float>( _VPLs.size() );
		//Ajouter la couleur L et l'atténuation w dans une classe VPL héritant de PointLight
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