#ifndef __QUAD_LIGHT_HPP__
#define __QUAD_LIGHT_HPP__

#include "base_light.hpp"
#include "utils/random.hpp"

namespace RT_ISICG
{
	class QuadLight : public BaseLight
	{
	  public:
		QuadLight()			 = delete;
		virtual ~QuadLight() = default;

		QuadLight( const Vec3f & p_position,
				   const Vec3f & p_u,
				   const Vec3f & p_v,
				   const Vec3f & p_color,
				   const float	 p_power = 1.f )
			: BaseLight( p_color, p_power ), _position( p_position ), _u( p_u ), _v( p_v ),
			  _n( glm::cross( p_u, p_v ) ), _area( glm::length( p_u ) * glm::length( p_v ) )
		{
			_isSurface = true;
		};

		virtual LightSample sample( const Vec3f & p_point ) const;

	  private:
		Vec3f _position;
		Vec3f _u;
		Vec3f _v;
		Vec3f _n;
		float _area;
	};
} // namespace RT_ISICG

#endif // !__QUAD_LIGHT_HPP__
