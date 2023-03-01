#include "bvh.hpp"
#include "geometry/triangle_mesh_geometry.hpp"
#include "utils/chrono.hpp"

namespace RT_ISICG
{
	void BVH::build( std::vector<TriangleMeshGeometry> * p_triangles )
	{
		std::cout << "Building BVH..." << std::endl;
		if ( p_triangles == nullptr || p_triangles->empty() )
		{
			throw std::exception( "BVH::build() error: no triangle provided" );
		}
		_triangles = p_triangles;

		Chrono chr;
		chr.start();

		/// TODO

		chr.stop();

		std::cout << "[DONE]: " << chr.elapsedTime() << "s" << std::endl;
	}

	bool BVH::intersect( const Ray & p_ray, const float p_tMin, const float p_tMax, HitRecord & p_hitRecord ) const
	{
		/// TODO
		return false;
	}

	bool BVH::intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		/// TODO
		return false;
	}

	void BVH::_buildRec( BVHNode *			p_node,
						 const unsigned int p_firstTriangleId,
						 const unsigned int p_lastTriangleId,
						 const unsigned int p_depth )
	{

		for ( size_t i = p_firstTriangleId; i < p_lastTriangleId; i++ )
		{
			const TriangleMeshGeometry tr = _triangles->at( i );
			p_node->_aabb.extend(tr.getVertex0());
			p_node->_aabb.extend(tr.getVertex1());
			p_node->_aabb.extend(tr.getVertex2());
		}

		int nbTriangles = ( p_lastTriangleId - p_firstTriangleId );

		if (nbTriangles < _maxTrianglesPerLeaf || p_depth > _maxDepth)
		{
			size_t largestAxis = p_node->_aabb.largestAxis();
			Vec3f  centre	   = p_node->_aabb.centroid();
			float  milieu	   = -1.f;

			if ( largestAxis == 0 )
				milieu = centre.x;
			else if ( largestAxis == 1 )
				milieu = centre.y;
			else
				milieu = centre.z;


		}
	}

	bool BVH::_intersectRec( const BVHNode * p_node,
							 const Ray &	 p_ray,
							 const float	 p_tMin,
							 const float	 p_tMax,
							 HitRecord &	 p_hitRecord ) const
	{
		/// TODO
		return false;
	}
	bool BVH::_intersectAnyRec( const BVHNode * p_node,
								const Ray &		p_ray,
								const float		p_tMin,
								const float		p_tMax ) const
	{
		/// TODO
		return false;
	}

	void BVH::_partition( const size_t		 p_axePartition,
						  const float		 p_milieu,
						  const unsigned int p_firstTriangleId,
						  const unsigned int p_lastTriangleId ) const
	{
		for (size_t i = p_firstTriangleId; i < p_lastTriangleId; i++)
		{
			const TriangleMeshGeometry tr = _triangles->at( i );
		}
	}
} // namespace RT_ISICG
