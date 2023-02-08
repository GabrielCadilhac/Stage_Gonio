#include "point_light.hpp"

namespace RT_ISICG
{
	LightSample PointLight::sample(const Vec3f & p_point) const
	{
		Vec3f direction = _position - p_point;
		float distance	= glm::distance( _position, p_point );
		direction		= glm::normalize( direction );

		Vec3f radiance	= _color * _power * ( 1 / ( distance * distance ) );

		return LightSample( direction, distance, radiance, 1.f );
	}
}