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