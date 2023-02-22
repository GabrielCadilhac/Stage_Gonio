#include "perspective_camera.hpp"
#include <glm/gtx/string_cast.hpp>

namespace RT_ISICG
{
	PerspectiveCamera::PerspectiveCamera( const float p_aspectRatio ) : _aspectRatio( p_aspectRatio )
	{
		_updateViewport();
	}

	PerspectiveCamera::PerspectiveCamera( const Vec3f & p_position,
										  const Vec3f & p_lookAt,
										  const Vec3f & p_up,
										  const float	p_fovy,
										  const float	p_aspectRatio )
		: BaseCamera( p_position ), _fovy( p_fovy ), _aspectRatio( p_aspectRatio )
	{
		_w = glm::normalize( p_position - p_lookAt);
		_u = glm::cross( p_up, _w );
		_v = glm::cross( _w, _u );

		_updateViewport();
	}

	void PerspectiveCamera::_updateViewport()
	{
		const float height = 2.f * std::max(glm::tan( glm::radians( _fovy / 2.f ) ), 0.f) * _focalDistance;
		const float width  = height * _aspectRatio;

		_viewportU = _u * width;
		_viewportV = _v * height;

		_viewportTopLeftCorner = _position + _v * ( height / 2.f ) - _u * ( width / 2.f ) - _w * _focalDistance;
	}

} // namespace RT_ISICG
