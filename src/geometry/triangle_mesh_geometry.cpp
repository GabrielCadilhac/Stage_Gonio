#include "triangle_mesh_geometry.hpp"
#include "objects/triangle_mesh.hpp"

namespace RT_ISICG
{
	TriangleMeshGeometry::TriangleMeshGeometry( const unsigned int p_v0,
												const unsigned int p_v1,
												const unsigned int p_v2,
												MeshTriangle *	   p_refMesh )
		: _v0( p_v0 ), _v1( p_v1 ), _v2( p_v2 ), _refMesh( p_refMesh )
	{
		_faceNormal = glm::normalize( glm::cross( _refMesh->_vertices[ p_v1 ] - _refMesh->_vertices[ p_v0 ],
												  _refMesh->_vertices[ p_v2 ] - _refMesh->_vertices[ p_v0 ] ) );
		_aabb		= AABB( _refMesh->_vertices[ _v0 ], _refMesh->_vertices[ _v0 ] );
		_aabb.extend( _refMesh->_vertices[ _v1 ] );
		_aabb.extend( _refMesh->_vertices[ _v2 ] );
	}

	bool TriangleMeshGeometry::intersect( const Ray & p_ray, float & p_t, Vec3f & p_normal ) const
	{
		const Vec3f & o	 = p_ray.getOrigin();
		const Vec3f & d	 = p_ray.getDirection();
		const Vec3f & v0 = _refMesh->_vertices[ _v0 ];
		const Vec3f & v1 = _refMesh->_vertices[ _v1 ];
		const Vec3f & v2 = _refMesh->_vertices[ _v2 ];

		p_t = -1;
		p_normal = VEC3F_ZERO;

		const double EPSILON = 0.0001f;

		const Vec3f edge1(v1 - v0);
		const Vec3f edge2(v2 - v0);
		const Vec3f pVec  = glm::cross( d, edge2 );
		const float det   = glm::dot( edge1, pVec );

		if ( det > -EPSILON && det < EPSILON ) return false;

		const double invDet = 1.0 / det;

		// Check if the intersection if on the triangle (x axis)
		const Vec3f tVec = o - v0;
		const float u    = glm::dot( tVec, pVec ) * static_cast<float>(invDet);
		if ( u < 0.f || u > 1.f ) return false;

		// Check if the intersection if on the triangle (y axis)
		const Vec3f qVec = glm::cross( tVec, edge1 );
		const float v	 = glm::dot( d, qVec ) * static_cast<float>(invDet);
		if ( v < 0.f || u + v > 1.f ) return false;

		// If yes, we can compute distance to the triangle and return true
		const Vec3f & n0 = _refMesh->_normals[ _v0 ];
		const Vec3f & n1 = _refMesh->_normals[ _v1 ];
		const Vec3f & n2 = _refMesh->_normals[ _v2 ];

		p_t = glm::dot( edge2, qVec ) * static_cast<float>(invDet);

		p_normal = glm::normalize(( 1 - u - v ) * n0 + ( u * n1 ) + ( v * n2 ));

		return true;
	}

	const float TriangleMeshGeometry::distance( const float p_point, const unsigned int p_axis )
	{
		const Vec3f & vert0 = _refMesh->_vertices[ _v0 ];
		const Vec3f & vert1 = _refMesh->_vertices[ _v1 ];
		const Vec3f & vert2 = _refMesh->_vertices[ _v2 ];

		const Vec3f center = ( vert0 + vert1 + vert2 ) / 3.f;

		return center[ p_axis ] - p_point;
	}

} // namespace RT_ISICG
