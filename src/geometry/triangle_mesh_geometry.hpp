#ifndef __RT_ISICG_TRIANGLE_GEOMETRY__
#define __RT_ISICG_TRIANGLE_GEOMETRY__

#include "base_geometry.hpp"
#include "aabb.hpp"

namespace RT_ISICG
{
	class MeshTriangle;

	class TriangleMeshGeometry : public BaseGeometry
	{
	  public:
		TriangleMeshGeometry()			= delete;
		virtual ~TriangleMeshGeometry() = default;

		TriangleMeshGeometry( const unsigned int p_v0,
							  const unsigned int p_v1,
							  const unsigned int p_v2,
							  MeshTriangle *	 p_refMesh );

		bool intersect( const Ray & p_ray, float & p_t, float p_u, float p_v ) const;

		inline const Vec3f & getFaceNormal() const { return _faceNormal; };
		inline const AABB  & getAABB() const { return _aabb; };
		inline MeshTriangle * getRefMesh() const { return _refMesh; };

		const Vec3f getInterNormal( const float p_u, const float p_v ) const;

	  private:
		MeshTriangle * _refMesh;
		union
		{
			struct
			{
				unsigned int _v0, _v1, _v2;
			};
			unsigned int _v[ 3 ] = { 0, 0, 0 };
		};

		Vec3f _faceNormal;
		AABB  _aabb = AABB(VEC3F_ZERO, VEC3F_ZERO);
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_TRIANGLE_GEOMETRY__
