#ifndef __POINT_LIGHT_HPP__
#define __POINT_LIGHT_HPP__

#include "base_light.hpp"
#include "utils/random.hpp"

#include "ray.hpp"
#include "hit_record.hpp"
#include "scene.hpp"

namespace RT_ISICG
{
	class PointLight : public BaseLight
	{
	  public:
		PointLight()		  = delete;
		virtual ~PointLight() = default;

		PointLight( const Vec3f & p_position, const Vec3f & p_color, const float p_power = 1.f )
			: BaseLight( p_color, p_position, p_power ) {};

		virtual LightSample sample( const Vec3f & p_point ) const;
	};
} // namespace RT_ISICG

#endif // __POINT_LIGHT_HPP__
