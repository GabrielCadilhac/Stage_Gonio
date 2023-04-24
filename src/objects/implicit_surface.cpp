#include "implicit_surface.hpp"

namespace RT_ISICG
{
	bool ImplicitSurface::intersect( const Ray & p_ray,
									 const float p_tMin,
									 const float p_tMax,
									 HitRecord & p_hitRecord ) const
	{
		float t = p_tMin;
		int   i = 0;
		while ( t < p_tMax && i < 128 )
		{
			const Vec3f pos	 = p_ray.getOrigin() + p_ray.getDirection() * t;
			const float dist = _sdf( pos );

			if ( glm::abs(dist) < _minDistance )
			{
				p_hitRecord._point	= pos;
				p_hitRecord._normal = _evaluateNormal( p_hitRecord._point );
				p_hitRecord.faceNormal( p_ray.getDirection() );
				p_hitRecord._distance = t;
				p_hitRecord._object	  = this;
				return true;
			}

			t += dist;
			i++;
		}
		return false;
	}

	bool ImplicitSurface::intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		float t = p_tMin;
		int	  i = 0;
		while ( t < p_tMax && i < 128 )
		{
			const Vec3f pos	 = p_ray.getOrigin() + p_ray.getDirection() * t;
			const float dist = _sdf( pos );

			if ( dist < _minDistance ) return true;

			t += dist;
			i++;
		}
		return false;
	}
} // namespace RT_ISICG
