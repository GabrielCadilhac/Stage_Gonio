#ifndef __RT_ISICG_QUAD_LIGHT_HPP__
#define __RT_ISICG_QUAD_LIGHT_HPP__

#include "base_light.hpp"
#include "utils/random.hpp"
#include "ray.hpp"

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
			: BaseLight( p_color, p_position, p_power ), _u( p_u ), _v( p_v ),
			  _n( glm::normalize( glm::cross( p_u, p_v ) ) ), _area( glm::length( p_u ) * glm::length( p_v ) )
		{
			_isSurface = true;
		};

		virtual LightSample sample( const Vec3f & p_point ) const;

		virtual Ray sampleLightRay( const float p_theta, const float p_phi ) const;

	  private:
		Vec3f _u;
		Vec3f _v;
		Vec3f _n;
		float _area;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_QUAD_LIGHT_HPP__
