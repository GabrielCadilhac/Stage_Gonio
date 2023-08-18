#include "plane.hpp"

namespace RT_ISICG
{
	bool Plane::intersect(const Ray& p_ray, const float p_tMin, const float p_tMax, HitRecord& p_hitRecord) const {
		float t;

		if (_geometry.intersect(p_ray, t))
		{
			if ( t > p_tMax ) { return false; }; //Vérifier que l'intersection est suffisemment proche
			if ( t < p_tMin ) { return false; }; //Mais pas trop

			p_hitRecord._point = p_ray.pointAtT( t );
			p_hitRecord._normal = _geometry.getNormal();
			p_hitRecord.faceNormal( p_ray.getDirection() );
			p_hitRecord._distance = t;
			p_hitRecord._object	  = this;

			// Compute UV coordinates
			const Vec3f a = glm::cross( _geometry.getNormal(), Vec3f( 1.f, 0.f, 0.f ) );
			const Vec3f b = glm::cross( _geometry.getNormal(), Vec3f( 0.f, 1.f, 0.f ) );
			const Vec3f c = glm::cross( _geometry.getNormal(), Vec3f( 0.f, 0.f, 1.f ) );
			
			const Vec3f maxAB = glm::dot( a, a ) < glm::dot( b, b ) ? b : a;
			const Vec3f u	  = glm::normalize( glm::dot( maxAB, maxAB ) < glm::dot( c, c ) ? c : maxAB );
			const Vec3f v	  = glm::cross( _geometry.getNormal(), u );

			p_hitRecord._u = glm::dot(u, p_hitRecord._point );
			p_hitRecord._v = glm::dot(v, p_hitRecord._point );

			return true;
		}
		return false;
	}

	bool Plane::intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		float t;

		if ( _geometry.intersect( p_ray, t ) )
			return ( t <= p_tMax && t >= p_tMin );
		return false;
	}
}