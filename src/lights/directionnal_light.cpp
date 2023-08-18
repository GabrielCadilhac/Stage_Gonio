#include "directionnal_light.hpp"

namespace RT_ISICG
{
	LightSample DirectionnalLight::sample( const Vec3f & p_point ) const
	{
		return LightSample( -_direction, 1.f, _color * _power, 1.f );
	}

	Ray DirectionnalLight::sampleLightRay( const float p_theta, const float p_phi ) const
	{
		const Vec3f direction = glm::normalize( polarToCartesian( TWO_PIf * p_theta, glm::acos( 1.f - 2.f * p_phi ) ) );
		return Ray( _position, direction );
	}
} // namespace RT_ISICG