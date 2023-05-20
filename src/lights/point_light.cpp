#include "point_light.hpp"

namespace RT_ISICG
{
	LightSample PointLight::sample( const Vec3f & p_point ) const
	{
		Vec3f		direction = _position - p_point;
		const float distance  = glm::length( direction );
		direction			  = glm::normalize( direction );

		const Vec3f radiance = _color * _power * ( 1.f / ( distance * distance ) );

		return LightSample( direction, distance, radiance, 1.f );
	}

	Ray PointLight::sampleLightRay( const float p_theta, const float p_phi ) const
	{
		Vec3f direction
			= glm::normalize( polarToCartesian( TWO_PIf * p_theta, glm::acos( 1.f - 2.f * p_phi ) ) );
		return Ray( _position, direction );
	}
} // namespace RT_ISICG