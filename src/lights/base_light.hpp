#ifndef __RT_ISICG_BASE_LIGHT__
#define __RT_ISICG_BASE_LIGHT__

#include "light_sample.hpp"

namespace RT_ISICG
{
	class BaseLight
	{
	  public:
		BaseLight( const Vec3f & p_color, const Vec3f & p_position, const float p_power = 1.f )
			: _color( p_color ), _position( p_position ), _power( p_power )
		{
		}
		virtual ~BaseLight() = default;

		inline const Vec3f & getFlatColor() const { return _color; };
		inline const bool    getIsSurface() const { return _isSurface; };
		inline const Vec3f & getPosition()  const { return _position; };
		inline const float	 getPower()     const { return _power; };

		//virtual Ray sampleLightRay( const int p_i ) const = 0;
		virtual LightSample sample( const Vec3f & p_point ) const = 0;

	  protected:
		Vec3f _position	 = VEC3F_ZERO;
		Vec3f _color	 = WHITE;
		float _power	 = 1.f;
		bool  _isSurface = false;
	};

} // namespace RT_ISICG

#endif // __RT_ISICG_BASE_LIGHT__
