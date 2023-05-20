#include "kd_tree.hpp"
#include "light_tree.hpp"

namespace RT_ISICG
{
	float KDTree::_distance( const LightNode * p_light1, const LightNode * p_light2 ) const
	{
		return glm::distance( p_light1->_pointLight->getPosition(), p_light2->_pointLight->getPosition() );
	}

	KDNode * KDTree::_closerDistance( const LightNode * p_pivot, KDNode * p_light1, KDNode * p_light2 )
	{
		if ( p_light1 == nullptr && p_light2 == nullptr ) return nullptr;

		if ( p_light1 == nullptr || p_light1->_cluster->equal( p_pivot ) )
			return p_light2->_cluster->equal( p_pivot ) ? nullptr : p_light2;

		if ( p_light2 == nullptr || p_light2->_cluster->equal( p_pivot ) )
			return p_light1->_cluster->equal( p_pivot ) ? nullptr : p_light1;

		if ( !( p_light1->isActive || p_light2->isActive ) )
			return nullptr;
		else if ( !p_light1->isActive )
			return p_light2;
		else if ( !p_light2->isActive )
			return p_light1;

		const float distance1 = _distance( p_pivot, p_light1->_cluster );
		const float distance2 = _distance( p_pivot, p_light2->_cluster );

		if ( distance2 > distance1 ) return p_light1;

		return p_light2;
	}

	void KDTree::build( std::vector<LightNode*> * p_lights )
	{
		_lights = p_lights;

		_root = new KDNode();
		_buildRec( _root, 0, static_cast<unsigned int>( _lights->size() ), 0 );

	}

	KDNode * KDTree::_buildRec( KDNode *		   p_node,
								const unsigned int p_firstLightId,
								const unsigned int p_lastLightId,
								const int		   p_depth )
	{
		if ( p_firstLightId >= p_lastLightId ) return nullptr;

		// Choisir un axe en fonction de la profondeur
		const int axis = p_depth % 3;

		// Trier les lumières en fonction de l'axe choisie
		std::sort( _lights->begin() + p_firstLightId,
				   _lights->begin() + p_lastLightId,
				   [ axis ]( LightNode * ln1, LightNode * ln2 )
				   { return ln1->_pointLight->getPosition()[ axis ] < ln2->_pointLight->getPosition()[ axis ]; } );

		// On prend le milieu
		const int splitIndex = static_cast<int>( ( p_lastLightId + p_firstLightId ) / 2.f );

		// Et on applique la récursion sur les éléments triés
		p_node->_cluster = (*_lights)[ splitIndex ];
		(*_lights)[splitIndex]->_kdNode = p_node;

		p_node->_left  = new KDNode();
		p_node->_right = new KDNode();

		p_node->_left  = _buildRec( p_node->_left, p_firstLightId, splitIndex, p_depth + 1 );
		p_node->_right = _buildRec( p_node->_right, splitIndex + 1, p_lastLightId, p_depth + 1 );
		return p_node;
	}

	LightNode * KDTree::nearestNeighbor( LightNode * p_target )
	{
		KDNode * neighbor = _nearestNeighborRec( _root, p_target, nullptr, 0 );
		if ( neighbor == nullptr ) return nullptr;
		return neighbor->_cluster;
	}

	KDNode * KDTree::_nearestNeighborRec( KDNode *	  p_node,
											 LightNode * p_target,
											 KDNode *	 p_closestPoint,
											 const int	 p_depth )
	{
		if ( p_node == nullptr ) return p_closestPoint;

		const int axis				 = p_depth % 3;
		KDNode *	nextNode		 = nullptr;
		KDNode *	nextOppositeNode = nullptr;

		if ( p_target->_pointLight->getPosition()[ axis ] < p_node->_cluster->_pointLight->getPosition()[ axis ] )
		{
			nextNode		 = p_node->_left;
			nextOppositeNode = p_node->_right;
		}
		else
		{
			nextNode		 = p_node->_right;
			nextOppositeNode = p_node->_left;
		}

		p_closestPoint = _closerDistance(
			p_target, _nearestNeighborRec( nextNode, p_target, nullptr, p_depth + 1 ), p_node );
		
		if (p_closestPoint == nullptr) return nullptr;

		if ( _distance( p_target, p_closestPoint->_cluster )
			 > glm::abs( p_target->_pointLight->getPosition()[ axis ] - p_node->_cluster->_pointLight->getPosition()[axis] ) )
			p_closestPoint = _closerDistance(
				p_target, _nearestNeighborRec( nextOppositeNode, p_target, nullptr, p_depth + 1 ), p_closestPoint );

		return p_closestPoint;
	}

} // namespace RT_ISICG