#ifndef __RT_ISICG_IMPLICIT_BOX_FRAME__
#define __RT_ISICG_IMPLICIT_BOX_FRAME__

#include "implicit_surface.hpp"

namespace RT_ISICG
{
	class ImplicitBoxFrame : public ImplicitSurface
	{
	  public:
		ImplicitBoxFrame()			= delete;
		virtual ~ImplicitBoxFrame() = default;

		ImplicitBoxFrame( const std::string p_name,
						  const Vec3f &		p_center,
						  const Vec3f &		p_size,
						  const float		p_thickness )
			: ImplicitSurface( p_name ), _size( p_size ), _thickness( p_thickness ), _center(p_center) {};

		float _sdf( const Vec3f & p_point ) const override;

	  private:
		const Vec3f _size	   = VEC3F_ZERO;
		const Vec3f _center	   = VEC3F_ZERO;
		const float _thickness = 0.f;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_IMPLICIT_BOX_FRAME__