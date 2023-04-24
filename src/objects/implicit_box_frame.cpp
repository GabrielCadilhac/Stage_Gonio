#include "implicit_box_frame.hpp"

namespace RT_ISICG
{
	float ImplicitBoxFrame::_sdf( const Vec3f & p_point ) const
	{
		const Vec3f p = abs( p_point - _center ) - _size;
		const Vec3f q = abs( p + _thickness ) - _thickness;

		return glm::min( glm::min( glm::length( glm::max( Vec3f( p.x, q.y, q.z ), 0.0f ) )
									   + glm::min( glm::max( p.x, glm::max( q.y, q.z ) ), 0.0f ),
								   glm::length( glm::max( Vec3f( q.x, p.y, q.z ), 0.0f ) )
									   + glm::min( glm::max( q.x, glm::max( p.y, q.z ) ), 0.0f ) ),
						 glm::length( glm::max( Vec3f( q.x, q.y, p.z ), 0.0f ) )
							 + glm::min( glm::max( q.x, glm::max( q.y, p.z ) ), 0.0f ) );
	}
} // namespace RT_ISICG