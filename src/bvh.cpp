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
			const AABB aabb = ( *_triangles )[ i ].getAABB();
			_aabb.extend(aabb);
		}

		const unsigned int nbTriangles = ( p_lastTriangleId - p_firstTriangleId );

		if (nbTriangles < _maxTrianglesPerLeaf || p_depth > _maxDepth)
		{
			const size_t largestAxis = p_node->_aabb.largestAxis();
			const Vec3f  centre	   = p_node->_aabb.centroid();
			const float  milieu	   = centre[ largestAxis ];

			//if ( largestAxis == 0 )
			//	milieu = centre.x;
			//else if ( largestAxis == 1 )
			//	milieu = centre.y;
			//else
			//	milieu = centre.z;

			std::vector<TriangleMeshGeometry>::iterator pt
				= std::partition( _triangles->begin(),
								  _triangles->end(),
								  [ milieu, largestAxis ]( TriangleMeshGeometry triangle )
				{
					return triangle.distance( milieu, largestAxis ) < 0;
				} );

			_buildRec(p_node->_left, p_firstTriangleId, pt - _triangles->begin(), p_depth+1 );
			_buildRec(p_node->_right, pt - _triangles->begin(), p_lastTriangleId, p_depth+1 );
		}
	}

	bool BVH::_intersectRec( const BVHNode * p_node,
							 const Ray &	 p_ray,
							 const float	 p_tMin,
							 const float	 p_tMax,
							 HitRecord &	 p_hitRecord ) const
	{
		if ( p_node->isLeaf() ) 
		{
			if ( !p_node->_aabb.intersect( p_ray, p_tMin, p_tMax ) ) return false;

			float  tClosest = p_tMax;
			size_t hitTri	= (* _triangles).size();
			Vec3f  normal	= VEC3F_ZERO;

			for ( size_t i = 0; i < (*_triangles).size(); i++ )
			{
				float t;
				Vec3f n;

				if ( (*_triangles)[ i ].intersect( p_ray, t, n ) )
				{
					if ( t >= p_tMin && t <= tClosest )
					{
						tClosest = t;
						hitTri	 = i;
						normal	 = n;
					}
				}
				if ( hitTri != (* _triangles).size() ) // Intersection found.
				{
					p_hitRecord._point	= p_ray.pointAtT( tClosest );
					p_hitRecord._normal = normal;
					p_hitRecord.faceNormal( p_ray.getDirection() );
					p_hitRecord._distance = tClosest;
					p_hitRecord._object	  = ( *_triangles )[0];

					return true;
				}
			}
			return false;
		}
		

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
