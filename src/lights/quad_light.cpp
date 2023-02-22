#include "quad_light.hpp"

namespace RT_ISICG
{
	LightSample QuadLight::sample( const Vec3f & p_point ) const
	{
		const Vec3f randomPos = _position + randomFloat() * _u + randomFloat() * _v;

		Vec3f rayDirection   = randomPos - p_point;
		const float distance = glm::distance( randomPos, p_point );
		rayDirection	     = glm::normalize( rayDirection );

		const float cosAngle   = glm::dot( _n, rayDirection );
		const float invArea	   = 1.f / _area;
		const float newPDF	   = invArea * ( ( distance * distance ) / cosAngle );

		return LightSample( rayDirection, distance, ( _color * _power ) / newPDF, newPDF );
	}
} // namespace RT_ISICG