#include "whitted_integrator.hpp"

namespace RT_ISICG
{
	Vec3f WhittedIntegrator::Li( const Scene & p_scene,
								 const Ray &   p_ray,
								 const float   p_tMin,
								 const float   p_tMax ) const
	{
		return _trace( p_scene, p_ray, p_tMin, p_tMax, 0, false );
	}

	Vec3f WhittedIntegrator::_trace( const Scene & p_scene,
									 const Ray &   p_ray,
									 const float   p_tMin,
									 const float   p_tMax,
									 const int	   p_bounces,
									 const bool	   p_inObject ) const
	{
		HitRecord hitRecord;
		// S'il n'y a pas d'intersection avec la scène on renvoie la couleur de fond
		if ( !p_scene.intersect( p_ray, p_tMin, p_tMax, hitRecord ) ) return _backgroundColor;

		// Cas d'arrêt de la récursion pour éviter les réflexions infinie
		if ( p_bounces >= _nbBounces ) return BLACK;

		// Vérifier si l'objet est un mirroir
		if ( hitRecord._object->getMaterial()->isMirror() )
		{
			// Calculer le vecteur réfléchie
			const Vec3f reflectDirection = glm::normalize( glm::reflect( p_ray.getDirection(), hitRecord._normal ) );
			// Puis le rayon
			const Ray rayonReflexion( hitRecord._point, reflectDirection );
			// Pour réaliser un appel récursif sur le rayon réfléchie
			return _trace( p_scene, rayonReflexion, p_tMin, p_tMax, p_bounces + 1, p_inObject );
		}
		else if ( hitRecord._object->getMaterial()->isTransparent() )
		{
			const Vec3f reflectDirection = glm::normalize( glm::reflect( p_ray.getDirection(), hitRecord._normal ) );
			const Ray	rayReflexion( hitRecord._point, reflectDirection );
			const Vec3f reflectedColor = _trace( p_scene, rayReflexion, p_tMin, p_tMax, p_bounces + 1, p_inObject );

			float n1 = 1.f;
			float n2 = 1.f;
			if ( p_inObject )
				n1 = hitRecord._object->getMaterial()->getIOR();
			else
				n2 = hitRecord._object->getMaterial()->getIOR();

			const float eta = n1 / n2;
			const Vec3f refractDirection
				= glm::normalize( glm::refract( p_ray.getDirection(), hitRecord._normal, eta ) );
			const Ray	rayRefraction( hitRecord._point, refractDirection );
			const Vec3f refractedColor = _trace( p_scene, rayRefraction, p_tMin, p_tMax, p_bounces + 1, !p_inObject );

			const float rpar  = _f( hitRecord._normal, refractDirection, -p_ray.getDirection(), n2, n1 );
			const float rperp = _f( hitRecord._normal, refractDirection, -p_ray.getDirection(), n1, n2 );
			const float kr	  = 0.5f * ( glm::pow( rpar, 2.f ) + glm::pow( rperp, 2.f ) );
			
			return kr * reflectedColor + ( 1.f - kr ) * refractedColor;
		}
		else
			return _directLighting( p_scene, hitRecord, p_ray );
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