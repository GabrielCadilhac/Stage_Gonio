#include "generator_movement.hpp"
#include "texture.hpp"

// utils
#include "utils/convert.hpp"

namespace RT_ISICG
{
	MovementGenerator::MovementGenerator( const int p_seed ) : _seed( p_seed )
	{
		_movementEmitter = MovementEmitter();
		_dataSaver		 = DataSaver();
	}

	// Save previously generated data (movement data)
	void MovementGenerator::saveData( const std::string & p_dataPath )
	{
		_dataSaver.addVectorData( "camera", _primaryMovement, "positions" );
		_dataSaver.addVectorData( "light", _secondaryMovement, "positions" );

		_dataSaver.addFloat( "seed", static_cast<float>( _seed ) );

		_dataSaver.saveData( p_dataPath );
	}

	// Generate data
	// We can also use setPrimaryMovement(...) to import data from file
	void MovementGenerator::generateMovement( const Vec3f & p_offset,
											  const float	p_distance,
											  const Vec3f & p_range,
											  const bool	p_isPrimary,
											  const Vec2f & p_angle )
	{
		const float rangeSize = ( p_range.y - p_range.x );
		for ( int i = 0; i < static_cast<int>( p_range.z ); i++ )
		{
			// Adding small offset 0.0001f to prevent problem when camera is exactly above lookAt
			const float angle	 = ( static_cast<float>( i ) / p_range.z ) * rangeSize + p_range.x + 0.1f;
			Vec3f		position = VEC3F_ZERO;

			if ( p_angle.y != 0.f )
				position = polarToCartesian( angle * p_angle.x, angle * p_angle.y ) * p_distance + p_offset;
			else
				position = polarToCartesian( angle, PIf / 4.f ) * p_distance + p_offset;

			if ( p_isPrimary )
				_primaryMovement.push_back( position );
			else
				_secondaryMovement.push_back( position );
		}
	}
} // namespace RT_ISICG
