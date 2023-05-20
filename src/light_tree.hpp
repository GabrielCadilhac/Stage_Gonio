#ifndef __RT_ISICG_LIGHT_TREE__
#define __RT_ISICG_LIGHT_TREE__

#include "aabb.hpp"
#include "defines.hpp"
#include "lights/point_light.hpp"
#include "utils/random.hpp"
#include <queue>

namespace RT_ISICG
{
	struct KDNode;

	struct LightNode
	{
		LightNode( int p_id ) : clusterId( p_id ) {};
		LightNode() = default;
		~LightNode() {}

		inline bool isLeaf() const { return ( _left == nullptr ) && ( _right == nullptr ); };

		inline float _sumIntensities() const
		{
			if ( isLeaf() ) return _pointLight->getPower();

			float intensities = 0.f;
			if ( _left != nullptr ) 
			{ 
				intensities += _left->_sumIntensities(); 
			}
			if ( _right != nullptr )
			{
				intensities += _right->_sumIntensities();
			}

			return intensities;
		}

		inline float _diagonalLength( const LightNode * p_lightNode )
		{
			AABB newAABB;
			newAABB.extend( _aabb );
			newAABB.extend( p_lightNode->_aabb );
			return glm::length( newAABB.diagonal() );
		}

		inline float _computeMetric( const LightNode & p_lightNode )
		{
			AABB newAABB;
			newAABB.extend( _aabb );
			newAABB.extend( p_lightNode._aabb );
			float diagonalLength = glm::length( _aabb.diagonal() );
			float intensities	 = _sumIntensities() + p_lightNode._sumIntensities();
			return diagonalLength * intensities;
		}

		inline bool equal( const LightNode * p_other )
		{
			return _pointLight->getPosition() == p_other->_pointLight->getPosition();
		}

		inline bool operator==( const LightNode * p_other )
		{
			return _pointLight->getPosition() == p_other->_pointLight->getPosition();
		}

		KDNode * _kdNode = nullptr;

		LightNode * _left  = nullptr;
		LightNode * _right = nullptr;

		int clusterId = 0;

		//  Representative light of the cluster
		BaseLight * _pointLight = nullptr;
		AABB		_aabb;
	};

	class LightTree
	{
	  public:
		LightTree() = default;
		~LightTree() {}

		void build( std::vector<PointLight *> * p_lights );

		void breadthFirstSearch() const;

		LightNode * _root = nullptr;
	  private:
		void _print( std::priority_queue<std::tuple<float, LightNode *, LightNode *>,
										 std::vector<std::tuple<float, LightNode *, LightNode *>>,
										 std::greater<std::tuple<float, LightNode *, LightNode *>>> p_queue ) const;

		inline bool isClusterized( std::vector<int> p_lightNodes, const LightNode * p_lightNode ) const
		{
			int i = 0;
			bool isInCluster = false;
			while (i < p_lightNodes.size() && !isInCluster) {
				if ( p_lightNodes[ i ] == p_lightNode->clusterId ) isInCluster = true;
				i++;
			}
			return isInCluster;
		}


		LightNode * _merge( LightNode * p_lightNode1, LightNode * p_lightNode2, const int p_id ) const;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_LIGHT_TREE__
