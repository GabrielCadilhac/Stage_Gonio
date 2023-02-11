#include "renderer.hpp"
#include "integrators/ray_cast_integrator.hpp"
#include "integrators/direct_lighting_integrator.hpp"
#include "utils/console_progress_bar.hpp"
#include "utils/random.hpp"

namespace RT_ISICG
{
	Renderer::Renderer() { _integrator = new RayCastIntegrator(); }

	void Renderer::setIntegrator( const IntegratorType p_integratorType )
	{
		if ( _integrator != nullptr ) { delete _integrator; }

		switch ( p_integratorType )
		{
		case IntegratorType::RAY_CAST:
		default:
		{
			_integrator = new RayCastIntegrator();
			break;
		}
		case IntegratorType::DIRECT_LIGHTING:
			_integrator = new DirectLightingIntegrator();
			break;
		}
	}

	void Renderer::setBackgroundColor( const Vec3f & p_color )
	{
		if ( _integrator == nullptr ) { std::cout << "[Renderer::setBackgroundColor] Integrator is null" << std::endl; }
		else
		{
			_integrator->setBackgroundColor( p_color );
		}
	}

	float Renderer::renderImage( const Scene & p_scene, const BaseCamera * p_camera, Texture & p_texture )
	{
		const int width	 = p_texture.getWidth();
		const int height = p_texture.getHeight();

		const int distMin = 0.1f;
		const int distMax = 100.f;

		Chrono			   chrono;
		ConsoleProgressBar progressBar;

		//Raycast pour calculer la lumière du point d'intersection

		progressBar.start( height, 50 );
		chrono.start();

		const float pixelWidth  = ( 1.f / (float)( width  - 1.f ) );
		const float pixelHeight = ( 1.f / (float)( height - 1.f ) );

		#pragma omp parallel for
		for ( int j = 0; j < height; j++ )
		{
			#pragma omp parallel for
			for ( int i = 0; i < width; i++ )
			{
				//Ray ray = p_camera->generateRay(sx,sy);

				// Image Figure 1.a
				//p_texture.setPixel( i, j, Vec3f( sx, sy, 0.f ) );

				// Image Figure 1.b
				//p_texture.setPixel( i, j, ( ray.getDirection() + 1.f ) * 0.5f );

				// Image Figures 3 et 5
				//Vec3f color = glm::clamp(_integrator->Li( p_scene, ray, distMin, distMax ), 0.f, 1.f);
				//p_texture.setPixel( i, j, color );
				
				// Anti-Aliasing
				Vec3f couleur = VEC3F_ZERO;
				_integrator->setNbLightSamples( 4 );

				for (int k = 0; k < _nbPixelSamples; ++k)
				{
					const float sx = ( i + randomFloat() ) * pixelWidth;
					const float sy = ( j + randomFloat() ) * pixelHeight;

					const Ray	 ray  = p_camera->generateRay( sx, sy );
					couleur += _integrator->Li( p_scene, ray, distMin, distMax );
				}

				couleur /= _nbPixelSamples;
				couleur = glm::clamp( couleur, 0.f, 1.f );
				p_texture.setPixel( i, j, couleur );
				
			}
			progressBar.next();
		}

		chrono.stop();
		progressBar.stop();

		return chrono.elapsedTime();
	}
} // namespace RT_ISICG
