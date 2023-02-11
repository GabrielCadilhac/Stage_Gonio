#include "quad_light.hpp"

namespace RT_ISICG
{
	LightSample QuadLight::sample( const Vec3f & p_point ) const
	{
		Vec3f randomPos    = _position + randomFloat() * _u + randomFloat() * _v;

		Vec3f rayDirection = randomPos - p_point;
		float distance	   = glm::distance( randomPos, p_point );
		rayDirection	   = glm::normalize( rayDirection );

		float cosAngle	   = glm::dot( _n, rayDirection );
		float invArea	   = 1.f / _area;
		float newPDF	   = invArea * ( ( distance * distance ) / cosAngle );

		return LightSample( rayDirection, distance, ( _color * _power ) / newPDF, newPDF );
	}
} // namespace RT_ISICG