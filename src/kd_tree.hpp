#ifndef __RT_ISICG_KD_TREE__
#define __RT_ISICG_KD_TREE__

#include "defines.hpp"
#include <algorithm>
#include <vector>

namespace RT_ISICG
{
	struct LightNode;

	struct KDNode
	{
		KDNode() = default;
		~KDNode()
		{
			delete _left;
			delete _right;
		}

		bool isLeaf() const { return ( _left == nullptr && _right == nullptr ); }

		KDNode *	_left  = nullptr;
		KDNode *	_right = nullptr;
		LightNode * _cluster;

		bool isActive = true;
	};

	class KDTree
	{
	  public:
		KDTree() = default;
		~KDTree() { delete _root; }

		void build( std::vector<LightNode*> * p_lights );

		LightNode * nearestNeighbor( LightNode * p_target );

	  private:
		KDNode * _buildRec( KDNode *		   p_node,
							const unsigned int p_firstLightId,
							const unsigned int p_lastLightId,
							const int		   p_depth );

		KDNode * _nearestNeighborRec( KDNode *	  p_node,
									  LightNode * p_target,
									  KDNode *	  p_closestPoint,
									  const int	  p_depth );

		float _distance( const LightNode * p_light1, const LightNode * p_light2 ) const;

		KDNode * _closerDistance( const LightNode * p_pivot, KDNode * p_light1, KDNode * p_light2 );

		KDNode *				 _root	 = nullptr;
		std::vector<LightNode*> * _lights = nullptr;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_KD_TREE__