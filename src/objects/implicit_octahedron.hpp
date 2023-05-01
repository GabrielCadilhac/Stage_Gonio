#ifndef __RT_ISICG_IMPLICIT_OCTAHEDRON__
#define __RT_ISICG_IMPLICIT_OCTAHEDRON__

#include "implicit_surface.hpp"

namespace RT_ISICG
{
	class ImplicitOctahedron : public ImplicitSurface
	{
	  public:
		ImplicitOctahedron()		  = delete;
		virtual ~ImplicitOctahedron() = default;

		ImplicitOctahedron( const std::string p_name, const Vec3f & p_center, const float p_size )
			: ImplicitSurface( p_name ), _size( p_size ), _center( p_center ) {};

		float _sdf( const Vec3f & p_point ) const override;

	  private:
		const Vec3f _center = VEC3F_ZERO;
		const float _size	= 0.f;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_IMPLICIT_OCTAHEDRON__