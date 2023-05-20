#ifndef __RT_ISICG_IMPLICIT_HEXAGONAL_PRISM__
#define __RT_ISICG_IMPLICIT_HEXAGONAL_PRISM__

#include "implicit_surface.hpp"

namespace RT_ISICG
{
	class ImplicitHexagonalPrism : public ImplicitSurface
	{
	  public:
		ImplicitHexagonalPrism()		  = delete;
		virtual ~ImplicitHexagonalPrism() = default;

		ImplicitHexagonalPrism( const std::string p_name, const Vec3f & p_center, const Vec2f & p_size )
			: ImplicitSurface( p_name ), _size( p_size ), _center( p_center ) {};

		float _sdf( const Vec3f & p_point ) const override;

	  private:
		const Vec3f _center = VEC3F_ZERO;
		const Vec2f _size	= VEC2F_ZERO;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_IMPLICIT_HEXAGONAL_PRISM__
