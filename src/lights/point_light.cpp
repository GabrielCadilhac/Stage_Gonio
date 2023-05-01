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
} // namespace RT_ISICG