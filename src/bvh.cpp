#include "bvh.hpp"
#include "geometry/triangle_mesh_geometry.hpp"
#include "objects/triangle_mesh.hpp"
#include "utils/chrono.hpp"

#define USE_SAH

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

		_root = new BVHNode();
		_buildRec( _root, 0, static_cast<unsigned int>( _triangles->size() ), 0 );

		chr.stop();

		std::cout << "[DONE]: " << chr.elapsedTime() << "s" << std::endl;
	}

	bool BVH::intersect( const Ray & p_ray, const float p_tMin, const float p_tMax, HitRecord & p_hitRecord ) const
	{
		return _intersectRec( _root, p_ray, p_tMin, p_tMax, p_hitRecord );
	}

	bool BVH::intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		return _intersectAnyRec( _root, p_ray, p_tMin, p_tMax );
	}

	void BVH::_buildRec( BVHNode *			p_node,
						 const unsigned int p_firstTriangleId,
						 const unsigned int p_lastTriangleId,
						 const unsigned int p_depth )
	{
		p_node->_firstTriangleId = p_firstTriangleId;
		p_node->_lastTriangleId	 = p_lastTriangleId;

		for ( unsigned int i = p_firstTriangleId; i < p_lastTriangleId; i++ )
		{
			const AABB aabb = ( *_triangles )[ i ].getAABB();
			p_node->_aabb.extend( aabb );
		}

		const unsigned int nbTriangles = ( p_lastTriangleId - p_firstTriangleId );

		if ( nbTriangles > _maxTrianglesPerLeaf && p_depth < _maxDepth )
		{
			int			nbBuckets	 = _nbBuckets;
			int	axePartition = static_cast<int>( p_node->_aabb.largestAxis() );
			const int	splitIndex	 = _bestSplitIndex( p_node->_aabb, p_firstTriangleId, p_lastTriangleId, axePartition );
			const Vec3f centre		 = p_node->_aabb.centroid();
			const float milieu		 = centre[ axePartition ];

			const std::vector<TriangleMeshGeometry>::iterator pt = std::partition(
				_triangles->begin() + p_firstTriangleId,
				_triangles->begin() + p_lastTriangleId,
				[ splitIndex, axePartition, p_node, nbBuckets, milieu ]( const TriangleMeshGeometry & triangle )
				{
#ifdef USE_SAH
					int b = static_cast<int>(
						nbBuckets * p_node->_aabb.relativePosition( triangle.getAABB().centroid() )[ axePartition ] );
					if ( b == nbBuckets ) b = nbBuckets - 1;
					return b <= splitIndex;
#else
					return triangle.getAABB().centroid()[ axePartition ] <= milieu;
#endif // USE_SAH
				} );

			const unsigned int idPartition = static_cast<unsigned int>( pt - _triangles->begin() );

			p_node->_left  = new BVHNode();
			p_node->_right = new BVHNode();

			_buildRec( p_node->_left, p_firstTriangleId, idPartition, p_depth + 1 );
			_buildRec( p_node->_right, idPartition, p_lastTriangleId, p_depth + 1 );
		}
	}

	bool BVH::_intersectRec( const BVHNode * p_node,
							 const Ray &	 p_ray,
							 const float	 p_tMin,
							 const float	 p_tMax,
							 HitRecord &	 p_hitRecord ) const
	{

		if ( !p_node->_aabb.intersect( p_ray, p_tMin, p_tMax ) ) return false;

		if ( p_node->isLeaf() )
		{

			float  tClosest = p_tMax;
			float  uClosest = 0.f;
			float  vClosest = 0.f;
			size_t hitTri	= p_node->_lastTriangleId;

			if ( p_hitRecord._object != nullptr ) tClosest = p_hitRecord._distance;

			for ( size_t i = p_node->_firstTriangleId; i < p_node->_lastTriangleId; ++i )
			{
				float t;
				float u;
				float v;

				if ( ( *_triangles )[ i ].intersect( p_ray, t, u, v ) )
				{
					if ( t >= p_tMin && t <= tClosest )
					{
						tClosest = t;
						hitTri	 = i;
						uClosest = u;
						vClosest = v;
					}
				}
			}

			if ( hitTri != p_node->_lastTriangleId )
			{
				p_hitRecord._point	= p_ray.pointAtT( tClosest );
				p_hitRecord._normal = ( *_triangles )[ hitTri ].getInterNormal( uClosest, vClosest );
				p_hitRecord.faceNormal( p_ray.getDirection() );
				p_hitRecord._distance = tClosest;
				p_hitRecord._object	  = ( *_triangles )[ 0 ].getRefMesh();
				p_hitRecord._u		  = uClosest;
				p_hitRecord._v		  = vClosest;

				return true;
			}

			return false;
		}

		bool intersectedLeft  = _intersectRec( p_node->_left, p_ray, p_tMin, p_tMax, p_hitRecord );
		bool intersectedRight = _intersectRec( p_node->_right, p_ray, p_tMin, p_tMax, p_hitRecord );

		return intersectedLeft || intersectedRight;
	}

	bool BVH::_intersectAnyRec( const BVHNode * p_node,
								const Ray &		p_ray,
								const float		p_tMin,
								const float		p_tMax ) const
	{
		if ( !p_node->_aabb.intersect( p_ray, p_tMin, p_tMax ) ) return false;

		if ( p_node->isLeaf() )
		{
			for ( size_t i = p_node->_firstTriangleId; i < p_node->_lastTriangleId; i++ )
			{
				float t;
				float u = 0.f;
				float v = 0.f;
				if ( ( *_triangles )[ i ].intersect( p_ray, t, u, v ) )
				{
					if ( t >= p_tMin && t <= p_tMax ) return true;
				}
			}
			return false;
		}

		return _intersectAnyRec( p_node->_left, p_ray, p_tMin, p_tMax )
			   || _intersectAnyRec( p_node->_right, p_ray, p_tMin, p_tMax );
	}

	int BVH::_bestSplitIndex( const AABB &		 p_aabb,
							  const unsigned int p_firstTriangleId,
							  const unsigned int p_lastTriangleId,
							  const unsigned int p_dim ) const
	{
		Bucket * buckets = new Bucket[ _nbBuckets ];
		for ( int i = 0; i < _nbBuckets; ++i )
			buckets[ i ] = Bucket();

		// Compute buckets
		for ( unsigned int i = p_firstTriangleId; i < p_lastTriangleId; ++i )
		{
			TriangleMeshGeometry triangle		  = ( *_triangles )[ i ];
			Vec3f				 relativePosition = p_aabb.relativePosition( triangle.getAABB().centroid() );
			int					 b				  = static_cast<int>( _nbBuckets * relativePosition[ p_dim ] );
			if ( b == _nbBuckets ) b = _nbBuckets - 1;
			buckets[ b ].count++;
			buckets[ b ].bounds.extend( triangle.getAABB() );
		}

		// Compute costs
		float * costs = new float[ _nbBuckets ];
		for ( int i = 0; i < _nbBuckets - 1; ++i )
		{
			AABB b0, b1;
			int	 count0 = 0, count1 = 0;
			for ( int j = 0; j <= i; ++j )
			{
				b0.extend( buckets[ j ].bounds );
				count0 += buckets[ j ].count;
			}
			for ( int j = i + 1; j < _nbBuckets; ++j )
			{
				b1.extend( buckets[ j ].bounds );
				count1 += buckets[ j ].count;
			}
			costs[ i ]	= 0.125f + ( count0 * b0.surfaceArea() + count1 * b1.surfaceArea() ) / p_aabb.surfaceArea();
			float temp2 = costs[ i ];
		}

		// Search for the min cost
		float minCost			= costs[ 0 ];
		int	  minCostSplitIndex = 0;
		for ( int i = 1; i < _nbBuckets - 1; ++i )
		{
			if ( minCost > costs[ i ] )
			{
				minCost			  = costs[ i ];
				minCostSplitIndex = i;
			}
		}

		delete[] buckets;
		delete[] costs;

		return minCostSplitIndex;
	}
} // namespace RT_ISICG