#ifndef __RT_ISICG_DISC_LIGHT__
#define __RT_ISICG_DISC_LIGHT__

#include "base_light.hpp"
#include "utils/random.hpp"

namespace RT_ISICG
{
	class DiskLight : public BaseLight
	{
	  public:
		DiskLight()			 = delete;
		virtual ~DiskLight() = default;

		DiskLight( const Vec3f & p_position,
				   const Vec3f & p_color,
				   const Vec3f & p_u,
				   const Vec3f & p_v,
				   const float	 p_radius,
				   const float	 p_power = 1.f )
			: BaseLight( p_color, p_position, p_power ), _radius( p_radius ), _u( p_u ), _v( p_v ),
			  _area( PIf * glm::pow( _radius, 2.f ) ), _normal( glm::normalize( glm::cross( _u, _v ) ) )
		{
			_isSurface = true;
		};

		virtual LightSample sample( const Vec3f & p_point ) const;

	  private:
		Vec2f _sampleGeometry() const;
		Vec3f _u;
		Vec3f _v;
		float _radius;
		Vec3f _normal;
		float _area;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_DISC_LIGHT__
