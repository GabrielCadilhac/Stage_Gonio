#include "hexagonal_prism_implicit.hpp"

namespace RT_ISICG
{
	float ImplicitHexagonalPrism::_sdf( const Vec3f & p_point ) const
	{
		Vec3f		p = abs( p_point - _center );
		const Vec3f k = Vec3f( -0.8660254, 0.5, 0.57735 );
		Vec2f		t = 2.f * glm::min( glm::dot( Vec2f( k.x, k.y ), Vec2f( p.x, p.y ) ), 0.f ) * Vec2f( k.x, k.y );
		p			  = Vec3f( p.x - t.x, p.y - t.y, p.z );
		Vec2f d		  = Vec2f(
			  glm::length( Vec2f( p.x, p.y ) - Vec2f( glm::clamp( p.x, -k.z * _size.x, k.z * _size.x ), _size.x ) )
							   * glm::sign( p.y - _size.x ),
						   p.z - _size.y );
		return glm::min( glm::max( d.x, d.y ), 0.f ) + glm::length( glm::max( d, 0.f ) );
	}
} // namespace RT_ISICG