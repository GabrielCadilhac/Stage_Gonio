#ifndef __RT_ISICG_IMPLICIT_SPHERE__
#define __RT_ISICG_IMPLICIT_SPHERE__

#include "implicit_surface.hpp"

namespace RT_ISICG
{
	class ImplicitSphere : ImplicitSurface
	{
		ImplicitSphere()  = delete;
		~ImplicitSphere() = default;

		ImplicitSphere( const std::string p_name, const Vec3f & p_center, const float p_radius )
			: ImplicitSurface( p_name ), _center( p_center ), _radius(p_radius) {};

		float _sdf( const Vec3f & p_point ) const override;

	  private:
		const Vec3f _center = VEC3F_ZERO;
		const float _radius = 0.f;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_IMPLICIT_SPHERE__