#include "implicit_box_frame.hpp"

namespace RT_ISICG
{
	float ImplicitBoxFrame::_sdf( const Vec3f & p_point ) const
	{
		const Vec3f temp = ( p_point - Vec3f( 0.f, 1.f, 0.f ) );
		const Vec3f p	 = abs( temp ) - _size;
		const Vec3f q	 = abs( temp + _thickness ) - _thickness;
		return glm::min( glm::min( 
			glm::length( glm::max( Vec3f( p.x, q.y, q.z ), 0.f ) ) + glm::min( glm::max( p.x, glm::max( q.y, q.z ) ), 0.f ),
			glm::length( glm::max( Vec3f( q.x, p.y, q.z ), 0.f ) ) + glm::min( glm::max( q.x, glm::max( p.y, q.z ) ), 0.f ) ),
			glm::length( glm::max( Vec3f( q.x, q.y, p.z ), 0.f ) ) + glm::min( glm::max( q.x, glm::max( q.y, p.z ) ), 0.f ) );
	}
} // namespace RT_ISICG