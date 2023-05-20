#include "utils/chrono.hpp"
#include "light_tree.hpp"
#include "kd_tree.hpp"

namespace RT_ISICG
{
	void LightTree::build( std::vector<PointLight*> * p_lights )
	{
		std::cout << "Building LightTree..." << std::endl;
		if ( p_lights == nullptr || p_lights->empty() )
		{
			throw std::exception( "LightTree::build() error: no lights provided" );
		}

		Chrono chr;
		chr.start();

		std::vector<LightNode*> lightNodes;
		for ( int i = 0; i < p_lights->size(); ++i )
		{
			LightNode * lightNode = new  LightNode(i);
			lightNode->_pointLight = (*p_lights)[ i ];
			lightNode->_aabb		  = AABB();
			lightNode->_aabb.extend( (*p_lights)[ i ]->getPosition() );

			lightNodes.push_back( lightNode );
		}

		KDTree kdTree;
		kdTree.build( &lightNodes );

		std::priority_queue < std::tuple<float, LightNode *, LightNode *>,
			std::vector<std::tuple<float, LightNode *, LightNode *>>,
			std::greater<std::tuple<float, LightNode *, LightNode *>>> lightQueue;
		for ( LightNode * lightNode : lightNodes )
		{
			LightNode * neighbor = kdTree.nearestNeighbor( lightNode );
			lightQueue.push( std::make_tuple(lightNode->_diagonalLength(neighbor), lightNode, neighbor) );
		}

		int clusterId = static_cast<int>( lightNodes.size() );

		std::vector<int> clusterizedLightNode;
		while ( lightQueue.size() > 0 )
		{
			std::tuple<float, LightNode *, LightNode *> bestPair = lightQueue.top();
			lightQueue.pop();

			LightNode * lightNode1 = std::get<1>( bestPair );
			LightNode * lightNode2 = std::get<2>( bestPair );

			if ( lightNode1 != lightNode1->_kdNode->_cluster ) continue;

			if ( lightNode2 != lightNode2->_kdNode->_cluster )
			{
				LightNode * newNeighbor = kdTree.nearestNeighbor( lightNode1 );
				if ( newNeighbor == nullptr )
				{
					_root = lightNode1;
					continue;
				}
				lightQueue.push(
					std::make_tuple( lightNode1->_diagonalLength( newNeighbor ), lightNode1, newNeighbor ) );
				continue;
			}

			LightNode * mergedCluster = _merge( lightNode1, lightNode2, clusterId );
			LightNode * neighbor	  = kdTree.nearestNeighbor( mergedCluster );
			clusterId++;

			clusterizedLightNode.push_back( lightNode1->clusterId );
			clusterizedLightNode.push_back( lightNode2->clusterId );

			if ( neighbor == nullptr )
			{
				_root = mergedCluster;
				continue;
			}

			lightQueue.push( std::make_tuple( mergedCluster->_diagonalLength( neighbor ), mergedCluster, neighbor ) );
		}
		
		chr.stop();

		std::cout << "[DONE]: " << chr.elapsedTime() << "s" << std::endl;
	}

	void LightTree::breadthFirstSearch() const {
		std::queue<LightNode*> nodesQueue;
		nodesQueue.push( _root );

		while (!nodesQueue.empty())
		{
			LightNode * currentNode = nodesQueue.front();
			nodesQueue.pop();

			if (currentNode->isLeaf())
			{
				const Vec3f pos = currentNode->_pointLight->getPosition();
				std::cout << "Leaf : " << pos.x << " " << pos.y << " " << pos.z << std::endl;
			}
			else {
				if ( currentNode->_left != nullptr ) nodesQueue.push( currentNode->_left );
				if ( currentNode->_right != nullptr ) nodesQueue.push( currentNode->_right );
			}
		}
	}

	void LightTree::_print(
		std::priority_queue<std::tuple<float, LightNode *, LightNode *>,
							std::vector<std::tuple<float, LightNode *, LightNode *>>,
							std::greater<std::tuple<float, LightNode *, LightNode *>>> p_queue ) const
	{
		while (!p_queue.empty()) {
			std::tuple<float, LightNode *, LightNode *> qp = p_queue.top();
			std::cout << std::get<1>( qp )->clusterId << "," << std::get<2>( qp )->clusterId << " :("
					  << std::get<0>( qp ) << ") ";
			p_queue.pop();
		}
		std::cout << std::endl << "-----------" << std::endl;
	}

	LightNode * LightTree::_merge( LightNode * p_lightNode1, LightNode * p_lightNode2, const int p_id) const
	{

		LightNode * newLightNode = new LightNode(p_id);

		newLightNode->_left	 = p_lightNode1;
		newLightNode->_right = p_lightNode2;

		AABB newAABB;
		newAABB.extend( p_lightNode1->_aabb );
		newAABB.extend( p_lightNode2->_aabb );
		newLightNode->_aabb = newAABB;

		const float intensity1	= p_lightNode1->_pointLight->getPower();
		const float intensity2	= p_lightNode2->_pointLight->getPower();
		const float probability = intensity1 / ( intensity1 + intensity2 );

		if ( randomFloat() < probability )
		{
			//std::cout << "Merge node " << p_lightNode1->clusterId << " with " << p_lightNode2->clusterId << std::endl;
			newLightNode->_pointLight		= p_lightNode1->_pointLight;
			newLightNode->_kdNode			= p_lightNode1->_kdNode;
			p_lightNode1->_kdNode->_cluster = newLightNode;
			p_lightNode2->_kdNode->isActive = false;
		}
		else
		{
			//std::cout << "Merge node " << p_lightNode2->clusterId << " with " << p_lightNode1->clusterId << std::endl;
			newLightNode->_pointLight		= p_lightNode2->_pointLight;
			newLightNode->_kdNode			= p_lightNode2->_kdNode;
			p_lightNode2->_kdNode->_cluster = newLightNode;
			p_lightNode1->_kdNode->isActive = false;
		}

		return newLightNode;
	}
} // namespace RT_ISICG