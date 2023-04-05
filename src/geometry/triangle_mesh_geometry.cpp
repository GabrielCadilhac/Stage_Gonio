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

	bool TriangleMeshGeometry::intersect( const Ray & p_ray, float & p_t, float & p_u, float & p_v ) const
	{
		const Vec3f & o	 = p_ray.getOrigin();
		const Vec3f & d	 = p_ray.getDirection();
		const Vec3f & v0 = _refMesh->_vertices[ _v0 ];
		const Vec3f & v1 = _refMesh->_vertices[ _v1 ];
		const Vec3f & v2 = _refMesh->_vertices[ _v2 ];

		p_t = -1.f;

		const Vec3f edge1(v1 - v0);
		const Vec3f edge2(v2 - v0);
		const Vec3f pVec  = glm::cross( d, edge2 );
		const float det   = glm::dot( edge1, pVec );

		if ( det > -FLT_EPSILON && det < FLT_EPSILON ) return false;

		const float invDet = 1.f / det;

		// Check if the intersection if on the triangle (x axis)
		const Vec3f tVec(o - v0);
		const float u    = glm::dot( tVec, pVec ) * static_cast<float>(invDet);
		if ( u < 0.f || u > 1.f ) return false;

		// Check if the intersection if on the triangle (y axis)
		const Vec3f qVec(glm::cross( tVec, edge1 ));
		const float v	 = glm::dot( d, qVec ) * static_cast<float>(invDet);
		if ( v < 0.f || u + v > 1.f ) return false;

		// If yes, we can compute distance to the triangle and return true
		p_u = u;
		p_v = v;

		p_t = glm::dot( edge2, qVec ) * invDet;

		return true;
	}

	const Vec3f TriangleMeshGeometry::getInterNormal( const float p_u, const float p_v ) const
	{
		return glm::normalize( ( 1.f - p_u - p_v ) * _refMesh->_normals[ _v0 ] + ( p_u * _refMesh->_normals[ _v1 ] )
							   + ( p_v * _refMesh->_normals[ _v2 ] ) );
	}

} // namespace RT_ISICG
