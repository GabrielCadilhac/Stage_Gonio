#include "disk_light.hpp"

namespace RT_ISICG
{
	Vec2f DiskLight::_sampleGeometry() const
	{
		const Vec2f randomPoint = 2.f * Vec2f( randomFloat(), randomFloat() ) - Vec2f( -1.f );
		if ( randomPoint.x == 0.f && randomPoint.y == 0.f ) return randomPoint;

		float theta, r;
		if ( std::abs( randomPoint.x ) > std::abs( randomPoint.y ) )
		{
			r	  = randomPoint.x;
			theta = ( PIf / 4.f ) * ( randomPoint.y / randomPoint.x );
		}
		else
		{
			r	  = randomPoint.y;
			theta = ( PIf / 2.f ) - ( PIf / 4.f ) * ( randomPoint.x / randomPoint.y );
		}
		return r * Vec2f( glm::cos( theta ), glm::sin( theta ) );
	}

	LightSample DiskLight::sample( const Vec3f & p_point ) const
	{
		const Vec2f randomDiskPoint = _sampleGeometry();
		Vec3f		randomPos		= _position + _u * randomDiskPoint.x + _v * randomDiskPoint.y; 

		Vec3f		rayDirection = randomPos - p_point;
		const float distance	 = glm::length( rayDirection );
		rayDirection			 = glm::normalize( rayDirection );

		const float cosAngle = glm::dot( _normal, rayDirection );
		const float invArea	 = 1.f / _area;
		const float newPDF	 = invArea * ( ( distance * distance ) / cosAngle );

		return LightSample( rayDirection, distance, ( _color * _power ) / newPDF, newPDF );
	}
} // namespace RT_ISICG