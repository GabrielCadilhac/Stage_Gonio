#ifndef __RT_ISICG_MOVEMENT_GENERATOR__
#define __RT_ISICG_MOVEMENT_GENERATOR__

#include "defines.hpp"

//utils
#include "utils/data_saver.hpp"
#include "utils/noise_statistics.hpp"

//movement
#include "movement/emitter_movement.hpp"

//basics
#include "scene.hpp"
#include "cameras/perspective_camera.hpp"
#include "..\renderer.hpp"
#include "utils/console_progress_bar.hpp"

//std
#include <vector>

#define PROGRESS_BAR 0

namespace RT_ISICG
{
	class MovementGenerator
	{
	  public:
		MovementGenerator( const int p_seed );

		~MovementGenerator() = default;
		
		inline void setPrimaryMovement( const std::vector<Vec3f> & p_movement ) { _primaryMovement = p_movement; }
		inline void setSecondaryMovement( const std::vector<Vec3f> & p_movement ) { _secondaryMovement = p_movement; }
		inline void setImageSize( const Vec2i & p_size ) { _imageSize = p_size; }

		template<typename ListenerType, typename ValueType>
		inline void setPrimaryObject( const std::function<void( ValueType )> & p_callback )
		{
			_movementEmitter.on<ListenerType, ValueType>( "primary", p_callback );
		}

		template<typename ListenerType, typename ValueType>
		inline void setSecondaryObject( const std::function<void( ValueType )> & p_callback )
		{
			_movementEmitter.on<ListenerType, ValueType>( "secondary", p_callback );
		}

		inline void addData( const std::string & p_dataName, const std::map<std::string, Vec3f> & p_data )
		{
			_dataSaver.addMapData( p_dataName, p_data );
		}

		inline void addString(const std::string & p_dataName, const std::string & p_data)
		{
			_dataSaver.addString( p_dataName, p_data );
		}

		template<typename PrimaryType, typename SecondaryType>
		Texture render( const PerspectiveCamera & p_camera, const Scene & p_scene, const Renderer & p_renderer, const bool	p_bwRender = false)
		{
#if PROGRESS_BAR
			ConsoleProgressBar progressBar;
			progressBar.start( static_cast<int>( _primaryMovement.size() ) + 1, 50 );
#endif
			// Size of the final image, containing all renders
			const int texWidth	= static_cast<int>( _secondaryMovement.size() ) * _imageSize.x;
			const int texHeight = static_cast<int>( _primaryMovement.size() ) * _imageSize.y;

			const Texture finalImage { texWidth, texHeight, 3 };

			float renderTime = 0.f;

			// Do all renders, according to positions stored in (_primary/_secondary)Movement
			for ( int i = 0; i < static_cast<int>( _primaryMovement.size() ); i++ )
			{
				// Emit to call the moving function
				_movementEmitter.emit<PrimaryType>( "primary", _primaryMovement[ i ] );

				for ( int j = 0; j < static_cast<int>( _secondaryMovement.size() ); j++ )
				{
					_movementEmitter.emit<SecondaryType>( "secondary", _secondaryMovement[ j ] );

					// Render image
					//Texture image { _imageSize.x, _imageSize.y, 3 };
					//renderTime += p_renderer.renderImage( p_scene, &p_camera, image, false );
					//finalImage.addTexture( image, { j * _imageSize.x, i * _imageSize.y } );

					// Save images individually
					//const int index = i * static_cast<int>( _secondaryMovement.size() ) + j;
					//image.saveJpg( "./results/image" + std::to_string( index ) + ".jpg" );

					// Render pixel
					Vec3f	color = VEC3F_ZERO;
					renderTime += p_renderer.renderSample( p_scene, &p_camera, color );
					finalImage.setPixel( j, i, color / 255.f );

				}
#if PROGRESS_BAR
				progressBar.next();
#endif
			}

#if PROGRESS_BAR
			progressBar.stop();
#endif

			//std::cout << "Render time " << renderTime << std::endl;

			return finalImage;
		}

		void generateMovement( const Vec3f & p_offset,
							   const float	 p_distance,
							   const Vec3f & p_range,
							   const bool	 p_isPrimary,
							   const Vec2f & p_angle );

		void saveData( const std::string & p_dataPath );

	private:
		int _seed = 1;

		// Data
		DataSaver _dataSaver;
		Vec2i	  _imageSize = VEC2I_ZERO;

		// Positions
		std::vector<Vec3f> _primaryMovement;
		std::vector<Vec3f> _secondaryMovement;

		// Movement
		MovementEmitter _movementEmitter;
	};
} // namespace RT_ISICG

#endif //__RT_ISICG_MOVEMENT_GENERATOR__
