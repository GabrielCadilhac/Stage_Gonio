#ifndef __RT_ISICG_PERSPECTIVE_CAMERA__
#define __RT_ISICG_PERSPECTIVE_CAMERA__

#include "base_camera.hpp"
#include "utils/convert.hpp"

namespace RT_ISICG
{
	class PerspectiveCamera : public BaseCamera
	{
	  public:
		PerspectiveCamera( const float p_aspectRatio );

		PerspectiveCamera( const Vec3f & p_position,
						   const Vec3f & p_lookAt,
						   const Vec3f & p_up,
						   const float	 p_fovy,
						   const float	 p_aspectRatio );

		~PerspectiveCamera() override = default;

		inline Ray generateRay( const float p_sx, const float p_sy ) const override
		{
			const Vec3f rayViewport = _viewportTopLeftCorner + p_sx * _viewportU - p_sy * _viewportV;
			const Vec3f direction   = glm::normalize( rayViewport - _position );
			return Ray { _position, direction };
		}

		void lookAt( const Vec3f & p_lookAt, const Vec3f & p_up );

		inline void move( const Vec3f & p_lookAt, const float p_theta, const float p_phi, const float p_distance )
		{
			_position = polarToCartesian( p_theta, p_phi ) * p_distance + p_lookAt;
		}

		inline void changePosition( const Vec3f & p_newPosition )
		{
			_position = p_newPosition;
			lookAt( _lookAt, Vec3f( 0.f, 1.f, 0.f ) );
		}

		inline void rotate( const float p_theta, const float p_phi, const float p_distance )
		{
			move( _lookAt, p_theta, p_phi, p_distance );
			lookAt( _lookAt, Vec3f( 0.f, 1.f, 0.f ) );
		}

		inline void setPosition( const Vec3f & p_newPosition ) { _position = p_newPosition; }

	  private:
		void _updateViewport();

	  private:
		float _fovy			 = 60.f;
		float _focalDistance = 1.f;
		float _aspectRatio	 = 1.f;

		// Local coordinates system
		Vec3f _u = Vec3f( 1.f, 0.f, 0.f );
		Vec3f _v = Vec3f( 0.f, 1.f, 0.f );
		Vec3f _w = Vec3f( 0.f, 0.f, -1.f );

		// Viewport data
		Vec3f _viewportTopLeftCorner = VEC3F_ZERO; // Top left corner position
		Vec3f _viewportU			 = VEC3F_ZERO; // Horizontal vector
		Vec3f _viewportV			 = VEC3F_ZERO; // Vertical vector
	
		Vec3f _lookAt = VEC3F_ZERO;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_PERSPECTIVE_CAMERA__
