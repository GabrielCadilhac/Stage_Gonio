#include "quad_light.hpp"

namespace RT_ISICG
{
	LightSample QuadLight::sample( const Vec3f & p_point ) const
	{
		const Vec3f randomPos = _position + randomFloat() * _u + randomFloat() * _v;

		Vec3f rayDirection   = randomPos - p_point;
		const float distance = glm::length( rayDirection );
		rayDirection	     = glm::normalize( rayDirection );

		const float cosAngle   = glm::dot( _n, rayDirection );
		const float invArea	   = 1.f / _area;
		const float newPDF	   = invArea * ( ( distance * distance ) / cosAngle );

		return LightSample( rayDirection, distance, ( _color * _power ) / newPDF, newPDF );
	}

	Ray QuadLight::sampleLightRay( const float p_theta, const float p_phi ) const
	{
		const Vec3f randomPos = _position + randomFloat() * _u + randomFloat() * _v;
		const Vec3f direction = sampleHemisphere( -_n, p_theta, p_phi );
		return Ray( randomPos, direction );
	}

} // namespace RT_ISICG