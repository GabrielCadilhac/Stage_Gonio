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

		for (TriangleMeshGeometry triangle : (*_triangles))
		{
			AABB aabb = triangle.getAABB();
			_aabb.extend( aabb );
		}

		_buildRec( _root, 0, static_cast<unsigned int>(_triangles->size()), 0 );

		chr.stop();

		std::cout << "[DONE]: " << chr.elapsedTime() << "s" << std::endl;
	}

	bool BVH::intersect( const Ray & p_ray, const float p_tMin, const float p_tMax, HitRecord & p_hitRecord ) const
	{
		if ( !_aabb.intersect( p_ray, p_tMin, p_tMax ) ) return false;

		return _intersectRec( _root, p_ray, p_tMin, p_tMax, p_hitRecord );
	}

	bool BVH::intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
	{
		if ( !_aabb.intersect( p_ray, p_tMin, p_tMax ) ) return false;

		return _intersectAnyRec( _root, p_ray, p_tMin, p_tMax );
	}

	void BVH::_buildRec( BVHNode *			p_node,
						 const unsigned int p_firstTriangleId,
						 const unsigned int p_lastTriangleId,
						 const unsigned int p_depth )
	{
		p_node = new BVHNode();
		for ( unsigned int i = p_firstTriangleId; i < p_lastTriangleId; i++ )
		{
			const AABB aabb = ( *_triangles )[ i ].getAABB();
			p_node->_aabb.extend( aabb );
		}

		const unsigned int nbTriangles = ( p_lastTriangleId - p_firstTriangleId );

		if ( nbTriangles > _maxTrianglesPerLeaf && p_depth < _maxDepth )
		{
			const unsigned int largestAxis = static_cast<int>(p_node->_aabb.largestAxis());
			const Vec3f	 centre		 = p_node->_aabb.centroid();
			const float	 milieu		 = centre[ largestAxis ];

			std::vector<TriangleMeshGeometry>::iterator pt
				= std::partition( _triangles->begin(),
								  _triangles->end(),
								  [ milieu, largestAxis ]( TriangleMeshGeometry triangle )
								  { return triangle.distance( milieu, largestAxis ) > 0; } );

			const unsigned int newLastIndex = static_cast<unsigned int>(pt - _triangles->begin());
			_buildRec( p_node->_left, p_firstTriangleId, newLastIndex, p_depth + 1 );
			_buildRec( p_node->_right, newLastIndex, p_lastTriangleId, p_depth + 1 );
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
			size_t hitTri	= _triangles->size();
			Vec3f  normal	= VEC3F_ZERO;

			for ( size_t i = p_node->_firstTriangleId; i < p_node->_lastTriangleId; i++ )
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
			}

			if ( hitTri != _triangles->size() )
			{
				p_hitRecord._point	= p_ray.pointAtT( tClosest );
				p_hitRecord._normal = normal;
				p_hitRecord.faceNormal( p_ray.getDirection() );
				p_hitRecord._distance = tClosest;
				p_hitRecord._object	  = ( *_triangles )[ 0 ].getRefMesh();

				return true;
			}

			return false;
		}

		bool isIntersect = false;

		if ( p_node->_left != nullptr )
		{
			HitRecord temp;
			if (_intersectRec( p_node->_left, p_ray, p_tMin, p_tMax, temp ))
			{
				if ( temp._distance < p_hitRecord._distance )
				{
					p_hitRecord._point	  = temp._point;
					p_hitRecord._normal	  = temp._normal;
					p_hitRecord._distance = temp._distance;
					p_hitRecord._object	  = temp._object;
				}
				isIntersect = true;
			}
		}

		if ( p_node->_right != nullptr )
		{
			HitRecord temp;
			if ( _intersectRec( p_node->_right, p_ray, p_tMin, p_tMax, temp ) )
			{
				if ( temp._distance < p_hitRecord._distance )
				{
					p_hitRecord._point	  = temp._point;
					p_hitRecord._normal	  = temp._normal;
					p_hitRecord._distance = temp._distance;
					p_hitRecord._object	  = temp._object;
				}
				isIntersect = true;
			}
		}

		return isIntersect;
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
				Vec3f normal;
				if ( (*_triangles)[ i ].intersect( p_ray, t, normal ) )
				{
					if ( t >= p_tMin && t <= p_tMax ) return true;
				}
			}
		}

		if ( p_node->_left != nullptr )
		{
			if ( _intersectAnyRec( p_node->_left, p_ray, p_tMin, p_tMax ) )
				return true;
		}

		if ( p_node->_right != nullptr )
		{
			if ( _intersectAnyRec( p_node->_right, p_ray, p_tMin, p_tMax ) )
				return true;
		}

		return false;
	}
} // namespace RT_ISICG
