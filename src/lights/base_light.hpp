#ifndef __RT_ISICG_BASE_LIGHT__
#define __RT_ISICG_BASE_LIGHT__

#include "light_sample.hpp"
#include "ray.hpp"
#include "utils/convert.hpp"

namespace RT_ISICG
{
	class BaseLight
	{
	  public:
		BaseLight( const Vec3f & p_color, const Vec3f & p_position, const float p_power = 1.f )
			: _initialPosition( p_position ), _position( p_position ), _color( p_color ), _power( p_power )
		{
		}
		virtual ~BaseLight() = default;

		inline const Vec3f & getFlatColor() const { return _color; }
		inline bool			 getIsSurface() const { return _isSurface; }
		inline const Vec3f & getPosition()  const { return _position; }
		inline float		 getPower()     const { return _power; }
		inline void			 setPosition( const Vec3f & p_position ) { _position = p_position; }

		inline void polarPosition( const float p_theta, const float p_phi, const float p_distance )
		{
			_position = polarToCartesian( p_theta, p_phi ) * p_distance + _initialPosition;
		}

		virtual Ray			sampleLightRay( const float p_theta, const float p_phi ) const = 0;
		virtual LightSample sample( const Vec3f & p_point ) const						   = 0;

	  protected:
		Vec3f _initialPosition = VEC3F_ZERO;
		Vec3f _position		   = VEC3F_ZERO;
		Vec3f _color		   = WHITE;
		float _power		   = 1.f;
		bool  _isSurface	   = false;
	};

} // namespace RT_ISICG

#endif // __RT_ISICG_BASE_LIGHT__
