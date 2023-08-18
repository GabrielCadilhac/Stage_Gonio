#include "cameras/perspective_camera.hpp"
#include "defines.hpp"

// movement
#include "movement/generator_movement.hpp"

// renderer
#include "renderer.hpp"

// utils
#include "utils/data_saver.hpp"
#include "utils/noise_statistics.hpp"
#include "utils/console_progress_bar.hpp"

// std
#include <iostream>
#include <map>
#include <direct.h>

#define COMPUTE_STATS 1

enum
{
	NB_SAMPLE_PRIMARY	= 10,
	NB_SAMPLE_SECONDARY = 10,
	NB_SAMPLE_NOISE     = 32,
	SEED				= 1
};

namespace RT_ISICG
{
	int main( int argc, char ** argv )
	{
		constexpr int imgWidth	= 1;
		constexpr int imgHeight = 1;

		constexpr float aspectRatio = static_cast<float>( imgWidth ) / imgHeight;

		const std::string imagePath = "results\\image";

		// Texture properties
		constexpr float textureSize	 = 128.f;
		constexpr float textureScale = textureSize / 10.f;

		// Movement properties
		constexpr Vec3f cameraLookAt(
			-textureSize / ( 2.f * textureScale ), 0.f, textureSize / ( 2.f * textureScale ) );
		constexpr float cameraDistance = 10.f;

		constexpr Vec3f lightDistance( 0.f, 4.f, 0.f );
		constexpr Vec3f lightFirstPosition = { cameraLookAt.x, 4.f, cameraLookAt.z };

		// Create and init scene.
		Scene scene;
		scene.setTextureScale( textureScale );
		scene.init_stage( Vec2i( static_cast<int>( textureSize ) ), lightFirstPosition, SEED );

		// Create camera et place it with .rotate()
		PerspectiveCamera camera( Vec3f( 0.f, 0.f, 0.f ), cameraLookAt, Vec3f( 0.f, 1.f, 0.f ), 60.f, aspectRatio );

		// Init camera position and look manually
		// camera.rotate( 0.f, ( PIf / 4.f ), 2.f );
		// camera.setPosition( Vec3f( 1.f, 2.f, imageSize / ( 2.f * scaling ) ) );
		// camera.lookAt( cameraLookAt, Vec3f( 0.f, 1.f, 0.f ) );

		// Create and setup the renderer.
		Renderer renderer;
		renderer.setIntegrator( IntegratorType::DIRECT_LIGHTING );
		renderer.setBackgroundColor( BLACK );
		renderer.setNbPixelSamples( 4 );
		renderer.setNbLightSamples( 1 );
		
		// Init movements
		MovementGenerator generator( SEED );

		// Init objects to move, here camera will move first and second light
		generator.setPrimaryObject<PerspectiveCamera, const Vec3f>( [ &camera ]( const Vec3f & p_value ) -> void
																	{ camera.changePosition( p_value ); } );

		generator.setSecondaryObject<Scene, const Vec3f>(
			[ &scene ]( const Vec3f & p_position ) -> void { scene.moveLight( p_position, 0 ); } );

		// Init generator variables
		generator.setImageSize( { imgWidth, imgHeight } );

		// Init movement (start, end, step)
		const Vec3f cameraRange( -PIf / 2.f, PIf / 2.f, NB_SAMPLE_PRIMARY );
		const Vec3f lightRange( 0.f, PIf, NB_SAMPLE_SECONDARY );

		//Generate a movement or set it from other generation
		generator.generateMovement( cameraLookAt, cameraDistance, cameraRange, true, { 0.f, 1.f } );
		//generator.setPrimaryMovement( { Vec3f(- 13.f, 5.f, 5.f) } );

		generator.generateMovement( lightFirstPosition, lightDistance.y, lightRange, false, { 1.f, 0.f } );
		//generator.setSecondaryMovement( { lightFirstPosition } );

		// Launch rendering.
		std::cout << "Rendering..." << std::endl;
		std::cout << "- Image size: " << imgWidth << "x" << imgHeight << std::endl;
		
#if COMPUTE_STATS
		Texture texturesArray[ NB_SAMPLE_NOISE ];
#endif

		Texture finalImage { imgWidth * NB_SAMPLE_SECONDARY * NB_SAMPLE_NOISE,
							 imgHeight * NB_SAMPLE_PRIMARY * NB_SAMPLE_NOISE };

		ConsoleProgressBar progressBar;
		progressBar.start( NB_SAMPLE_NOISE * NB_SAMPLE_NOISE + 1, 50 );

		Chrono chrono;
		chrono.start();

		for ( int i = 0; i < NB_SAMPLE_NOISE; i++ )
		{
			for (int j = 0; j < NB_SAMPLE_NOISE; j++)
			{
				scene.textureSample( "ProceduralMaterial", i * NB_SAMPLE_NOISE + j );
				const Texture image = generator.render<PerspectiveCamera,Scene>( camera, scene, renderer, COMPUTE_STATS );

#if COMPUTE_STATS
				texturesArray[ i ]	= image;
#endif

				finalImage.addTexture( image, { i * image.getHeight(), j * image.getWidth() } );

				progressBar.next();
			}
		}
		chrono.stop();
		std::cout << "Render time " << std::to_string( chrono.elapsedTime() ) << std::endl;
		progressBar.stop();

		finalImage.saveJpg( "./results/finalImage.jpg" );

#if COMPUTE_STATS
		NoiseStatistics stats { texturesArray, NB_SAMPLE_NOISE };

		const Texture	meanTexture = stats.mean( false );
		meanTexture.saveJpg( "./results/mean.jpg" );

		const Texture varianceTexture = stats.variance();
		varianceTexture.saveJpg( "./results/variance.jpg" );

		const Texture stdTexture = stats.std();
		stdTexture.saveJpg( "./results/std.jpg" );

#endif

		// Add some others data
		std::map<std::string, Vec3f> cameraMap;
		std::map<std::string, Vec3f> lightMap;

		// Additional data to be saved in .json file
		cameraMap[ "distance" ] = { cameraDistance, 0.f, 0.f };
		cameraMap[ "lookAt" ]	= cameraLookAt;

		lightMap[ "distance" ]		= lightDistance;
		lightMap[ "firstPosition" ] = lightFirstPosition;

		generator.addData( "camera", cameraMap );
		generator.addData( "light", lightMap );

		char currentPath[ 200 ];
		_getcwd( currentPath, sizeof( currentPath ) );
		generator.addString( "image path", std::string(currentPath) + "\\" + imagePath );

		// Save images or evaluation in .json file
		generator.saveData( ".\\data\\example.json" );

		return EXIT_SUCCESS;
	}
} // namespace RT_ISICG

int main( int argc, char ** argv )
{
	try
	{
		return RT_ISICG::main( argc, argv );
	}
	catch ( const std::exception & e )
	{
		std::cerr << "Exception caught:" << std::endl << e.what() << std::endl;
	}
}
