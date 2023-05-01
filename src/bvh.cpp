#include "bvh.hpp"
#include "geometry/triangle_mesh_geometry.hpp"
#include "objects/triangle_mesh.hpp"
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
			const int	axePartition = static_cast<int>( p_node->_aabb.largestAxis() );
			//int			axePartition = -1;
			const int	splitIndex	 = -1; //_computeSAH( p_node->_aabb, p_firstTriangleId, p_lastTriangleId, axePartition );
			const Vec3f centre		 = p_node->_aabb.centroid();
			const float milieu		 = centre[ axePartition ];

			const std::vector<TriangleMeshGeometry>::iterator pt = std::partition(
				_triangles->begin() + p_firstTriangleId,
				_triangles->begin() + p_lastTriangleId,
				[ splitIndex, axePartition, p_node, nbBuckets, milieu ]( const TriangleMeshGeometry & triangle )
				{
					//int b = nbBuckets
					//		* static_cast<int>(
					//			p_node->_aabb.relativePosition( triangle.getAABB().centroid() )[ axePartition ] );
					//if ( b == nbBuckets ) b = nbBuckets - 1;
					//return b <= splitIndex;
					 return triangle.getAABB().centroid()[ axePartition ] <= milieu;
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

	int BVH::_computeSAH( const AABB &		 p_aabb,
						  const unsigned int p_firstTriangleId,
						  const unsigned int p_lastTriangleId,
						  int &				 p_dim ) const
	{
		const int index0 = _bestSplitIndex( p_aabb, p_firstTriangleId, p_lastTriangleId, 0 );
		const int index1 = _bestSplitIndex( p_aabb, p_firstTriangleId, p_lastTriangleId, 1 );
		const int index2 = _bestSplitIndex( p_aabb, p_firstTriangleId, p_lastTriangleId, 2 );
		const int index	 = glm::min( index0, glm::min( index1, index2 ) );

		if ( index == index0 )
			p_dim = 0;
		else if ( index == index1 )
			p_dim = 1;
		else
			p_dim = 2;

		return index;
	}

	int BVH::_bestSplitIndex( const AABB &		 p_aabb,
							  const unsigned int p_firstTriangleId,
							  const unsigned int p_lastTriangleId,
							  const unsigned int p_dim ) const
	{
		Bucket * buckets = new Bucket[ _nbBuckets ];

		// Compute buckets
		for ( unsigned int i = p_firstTriangleId; i < p_lastTriangleId; ++i )
		{
			TriangleMeshGeometry triangle = ( *_triangles )[ i ];
			int b = _nbBuckets * static_cast<int>( p_aabb.relativePosition( triangle.getAABB().centroid() )[ p_dim ] );
			if ( b == _nbBuckets ) b = _nbBuckets - 1;
			buckets[ b ].count++;
			buckets[ b ].bounds.extend( triangle.getAABB() );
		}

		// Compute costs
		float * costs = new float[ _nbBuckets ];
		for ( int i = 0; i < _nbBuckets - 1; ++i )
			costs[ i ] = 0.f;

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
			costs[ i ] += 0.125f + ( count0 * b0.surfaceArea() + count1 * b1.surfaceArea() ) / p_aabb.surfaceArea();
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

		return minCostSplitIndex;
	}
} // namespace RT_ISICG