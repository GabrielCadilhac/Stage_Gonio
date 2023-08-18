#ifndef __RT_ISICG_RENDERER__
#define __RT_ISICG_RENDERER__

#include "cameras/base_camera.hpp"
#include "defines.hpp"
#include "integrators/base_integrator.hpp"
#include "texture.hpp"
#include "integrators/ray_cast_integrator.hpp"
#include "utils/chrono.hpp"

namespace RT_ISICG
{
	class Renderer
	{
	  public:
		Renderer() { _integrator = new RayCastIntegrator(); }
		~Renderer() { delete _integrator; }

		void         setIntegrator( const IntegratorType p_integratorType );
		void         setBackgroundColor( const Vec3f & p_color ) const;
		float		 renderTexture( const Texture & p_texture, const int p_seed = 1 );

		inline void setNbPixelSamples( const int p_nbPixelSamples ) { _nbPixelSamples = p_nbPixelSamples; }
		inline void setNbLightSamples( const int p_nbLightSamples ) const
		{
			_integrator->setNbLightSamples( p_nbLightSamples );
		}

		float renderImage( const Scene &      p_scene,
		                   const BaseCamera * p_camera,
		                   const Texture &    p_texture,
		                   const bool         p_bwImage ) const;
		
		float renderSample( const Scene &	  p_scene, const BaseCamera * p_camera, Vec3f & p_sample ) const;

	  private:
		BaseIntegrator * _integrator	 = nullptr;
		int				 _nbPixelSamples = 1;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_RENDERER_2__
