#include "implicit_surface.hpp"

namespace RT_ISICG
{
	bool ImplicitSurface::intersect( const Ray & p_ray,
									 const float p_tMin,
									 const float p_tMax,
									 HitRecord & p_hitRecord ) const
	{
		Vec3f pos = p_ray.getOrigin();
		float step = p_tMin;
		float distance = p_tMin;

		while (step < p_tMax) {
			if (step <= 0) {
				p_hitRecord._point	= pos;
				//p_hitRecord._normal = _geometry.computeNormal( p_hitRecord._point );
				//p_hitRecord.faceNormal( p_ray.getDirection() );
				p_hitRecord._distance = distance;
				p_hitRecord._object	  = this;
				return true;
			}

			pos += p_ray.getDirection() * step;
			distance += p_ray.getDirection().length() * step;
			step = _sdf( pos );
		}

		return false;
	}

	bool ImplicitSurface::intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		Vec3f pos	   = p_ray.getOrigin();
		float step	   = p_tMin;

		while ( step < p_tMax )
		{
			if ( step <= 0 )
				return true;

			pos += p_ray.getDirection() * step;
			step = _sdf( pos );
		}
		return false;
	}
} // namespace RT_ISICG
