#include "cameras/perspective_camera.hpp"
#include "defines.hpp"
#include "renderer.hpp"

namespace RT_ISICG
{
	int main( int argc, char ** argv )
	{
		const int imgWidth	= 600;
		const int imgHeight = 400;

		const Vec3f cameraPosition( 0.f, 0.f, -2.f );
		const Vec3f cameraLookAt( 0.f, 0.f, 79.f);
		const float aspectRatio = float( imgWidth ) / imgHeight;

		// Create a texture to render the scene.
		Texture img = Texture( imgWidth, imgHeight );

		// Create and init scene.
		Scene scene;
		scene.init();

		// Create a perspective camera.
		//PerspectiveCamera camera( aspectRatio );

		//Nouvelle cam�ra avec le rep�re modifi� (Figure 5.d)
		PerspectiveCamera camera( cameraPosition, cameraLookAt, Vec3f(0.f, 1.f, 0.f), 60.f, aspectRatio );

		// Create and setup the renderer.
		Renderer renderer;
		renderer.setIntegrator( IntegratorType::DIRECT_LIGHTING );
		renderer.setBackgroundColor( GREY );
		//renderer.setNbPixelSamples( 4 );

		// Launch rendering.
		std::cout << "Rendering..." << std::endl;
		std::cout << "- Image size: " << imgWidth << "x" << imgHeight << std::endl;

		float renderingTime = renderer.renderImage( scene, &camera, img );

		std::cout << "-> Done in " << renderingTime << "ms" << std::endl;

		// Save rendered image.
		const std::string imgName = "image.jpg";
		img.saveJPG( RESULTS_PATH + imgName );

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