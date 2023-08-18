#include "renderer.hpp"
#include "integrators/direct_lighting_integrator.hpp"
#include "integrators/ray_cast_integrator.hpp"
#include "utils/console_progress_bar.hpp"
#include "utils/random.hpp"
#include "utils\noise.hpp"

namespace RT_ISICG
{
	void Renderer::setIntegrator( const IntegratorType p_integratorType )
	{
		delete _integrator;

		switch ( p_integratorType )
		{
		case IntegratorType::RAY_CAST:
		default:
		{
			_integrator = new RayCastIntegrator();
			break;
		}
		case IntegratorType::DIRECT_LIGHTING: _integrator = new DirectLightingIntegrator(); break;

		case IntegratorType::COUNT: break;
		}
	}

	void Renderer::setBackgroundColor( const Vec3f & p_color ) const
	{
		if ( _integrator == nullptr )
		{
			std::cout << "[Renderer::setBackgroundColor] Integrator is null" << std::endl;
		}
		else
		{
			_integrator->setBackgroundColor( p_color );
		}
	}

	float Renderer::renderTexture( const Texture & p_texture, const int p_seed )
	{
		const int width = p_texture.getWidth();
		const int height = p_texture.getHeight();

		Chrono	   chrono;
		chrono.start();
		const auto noise	 = new float[ static_cast<int>( width * height ) ];
		const auto normalMap = new Vec3f[ static_cast<int>( width * height ) ];
		
		Noise::perlinNoise( width, height, noise, p_seed );
		//Noise::convert2Normal( Vec2i( p_width, p_height ), noise, normalMap );

		for ( int i = 0; i < height; ++i )
		{
			for ( int j = 0; j < width; ++j )
				p_texture.setPixel( j, i, Vec3f( noise[ i * width + j ] ) );
		}

		chrono.stop();
		return chrono.elapsedTime();
	}


	float Renderer::renderImage( const Scene & p_scene, const BaseCamera * p_camera, const Texture & p_texture, const bool p_bwImage = false ) const
	{
		const int width	 = p_texture.getWidth();
		const int height = p_texture.getHeight();

		Chrono chrono;
		chrono.start();

		const float pixelWidth	= 1.f / ( static_cast<float>( width )  - 1.f );
		const float pixelHeight = 1.f / ( static_cast<float>( height ) - 1.f );

#pragma omp parallel for  // NOLINT(clang-diagnostic-source-uses-openmp)
		for ( int j = 0; j < height; j++ )
		{
			for ( int i = 0; i < width; i++ )
			{
				Vec3f color = VEC3F_ZERO;

				for ( int k = 0; k < _nbPixelSamples; ++k )
				{
					constexpr float distMax = 10000.f;
					constexpr float distMin = 0.0001f;

					const float sx = ( static_cast<float>( i ) + randomFloat() ) * pixelWidth;
					const float sy = ( static_cast<float>( j ) + randomFloat() ) * pixelHeight;

					const Ray ray = p_camera->generateRay( sx, sy );
					color += _integrator->Li( p_scene, ray, distMin, distMax );
				}
				
				color /= _nbPixelSamples;
				color = glm::clamp( color, 0.f, 1.f );

				// If black and white images
				if (p_bwImage)
					color = Vec3f { color.x + color.y + color.z } / 3.f;

				p_texture.setPixel( i, j, color );

			}
		}
		chrono.stop();
		return chrono.elapsedTime();
	}

	float Renderer::renderSample( const Scene & p_scene, const BaseCamera * p_camera, Vec3f & p_sample ) const
	{
		Chrono chrono;
		chrono.start();

		Vec3f color = VEC3F_ZERO;

		constexpr int width = 128;
		constexpr int height = 128;

		constexpr float distMax = 10000.f;
		constexpr float distMin = 0.0001f;

		for (int i = 0; i < height; i += 1)
		{
			for (int j = 0; j < width; j += 1)
			{
				const Ray ray = p_camera->generateRay( static_cast<float>( i ) / static_cast<float>( height ),
													   static_cast<float>( j ) / static_cast<float>( width ) );
				color += _integrator->Li( p_scene, ray, distMin, distMax );
			}
		}

		color /= width * height;
		color = glm::clamp( color, 0.f, 1.f );
		
		p_sample = glm::round( color * 255.f );

		chrono.stop();
		return chrono.elapsedTime();
	}

} // namespace RT_ISICG
