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
		// Cas d'arrêt de la récursion pour éviter les réflexions infinie
		if ( p_bounces >= _nbBounces ) return BLACK;

		// S'il n'y a pas d'intersection avec la scène on renvoie la couleur de fond
		HitRecord hitRecord;
		if ( !p_scene.intersect( p_ray, p_tMin, p_tMax, hitRecord ) ) return _backgroundColor;

		hitRecord.faceNormal( p_ray.getDirection() );

		// Vérifier si l'objet est un mirroir
		if ( hitRecord._object->getMaterial()->isMirror() )
		{
			// Calculer le vecteur réfléchie
			const Vec3f reflectDirection = glm::reflect( p_ray.getDirection(), hitRecord._normal );
			// Puis le rayon
			Ray rayonReflexion( hitRecord._point, reflectDirection );
			rayonReflexion.offset( hitRecord._normal );
			// Pour réaliser un appel récursif sur le rayon réfléchie
			return _trace( p_scene, rayonReflexion, 0.f, p_tMax, p_bounces + 1, p_inObject );
		}
		else if ( hitRecord._object->getMaterial()->isTransparent() )
		{
			const Vec3f reflectDirection = glm::reflect( p_ray.getDirection(), hitRecord._normal );
			Ray			rayReflexion( hitRecord._point, reflectDirection );
			rayReflexion.offset( hitRecord._normal );
			const Vec3f reflectedColor = _trace( p_scene, rayReflexion, 0.f, p_tMax, p_bounces + 1, p_inObject );

			float n1 = 1.f;
			float n2 = 1.f;
			if ( p_inObject )
				n1 = hitRecord._object->getMaterial()->getIOR();
			else
				n2 = hitRecord._object->getMaterial()->getIOR();

			const float eta		  = n1 / n2;
			const float cosThetaI = glm::max( glm::dot( -p_ray.getDirection(), hitRecord._normal ), 0.f );
			const float thetaI	  = glm::acos( cosThetaI );
			const float sqrtSin	  = glm::sqrt( 1.f - glm::pow( eta * glm::sin( thetaI ), 2.f ) );

			const float rs = glm::pow( ( n1 * cosThetaI - n2 * sqrtSin ) / ( n1 * cosThetaI + n2 * sqrtSin ), 2.f );
			const float rp = glm::pow( ( n1 * sqrtSin - n2 * cosThetaI ) / ( n1 * sqrtSin + n2 * cosThetaI ), 2.f );
			const float kr = 0.5f * ( rs + rp );

			if ( kr >= 1.f ) return reflectedColor;

			const Vec3f refractDirection = glm::refract( p_ray.getDirection(), hitRecord._normal, eta );

			Ray rayRefraction( hitRecord._point, refractDirection );
			rayRefraction.offset( -hitRecord._normal );
			const Vec3f refractedColor = _trace( p_scene, rayRefraction, 0.f, p_tMax, p_bounces + 1, !p_inObject );

			return kr * reflectedColor + ( 1.f - kr ) * refractedColor;
		}
		return _directLighting( p_scene, hitRecord, p_ray );
	}
} // namespace RT_ISICG