#include "implicit_octahedron.hpp"

namespace RT_ISICG
{
	float ImplicitOctahedron::_sdf( const Vec3f & p_point ) const
	{
		const Vec3f position = glm::abs( p_point - _center );
		const float m		 = position.x + position.y + position.z - _size;
		Vec3f  q;

		if ( 3.f * position.x < m )
			q = Vec3f( position.x, position.y, position.z );
		else if ( 3.f * position.y < m )
			q = Vec3f( position.y, position.z, position.x );
		else if ( 3.f * position.z < m )
			q = Vec3f( position.z, position.x, position.y );
		else
			return m * 0.57735027f;

		float k = glm::clamp( 0.5f * ( q.z - q.y + _size ), 0.f, _size );
		return length( Vec3f( q.x, q.y - _size + k, q.z - k ) );
	}
} // namespace RT_ISICG