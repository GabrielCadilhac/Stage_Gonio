#ifndef __RT_ISICG_DIRECTIONNAL_LIGHT__
#define __RT_ISICG_DIRECTIONNAL_LIGHT__

#include "base_light.hpp"

namespace RT_ISICG
{
	class DirectionnalLight : public BaseLight
	{
	  public:
		DirectionnalLight()	 = delete;
		~DirectionnalLight() = default;

		DirectionnalLight( const Vec3f & p_position,
						   const Vec3f & p_couleur,
						   const Vec3f & p_direction,
						   const float	 p_power = 1.f )
			: BaseLight( p_couleur, p_position, p_power ), _direction( glm::normalize( p_direction ) ) {};

		virtual LightSample sample( const Vec3f & p_point ) const;

		virtual Ray sampleLightRay( const float p_theta, const float p_phi ) const;

		private:
		Vec3f _direction = VEC3F_ZERO;
	};
} // namespace RT_ISICG
#endif // !__RT_ISICG_DIRECTIONNAL_LIGHT__
