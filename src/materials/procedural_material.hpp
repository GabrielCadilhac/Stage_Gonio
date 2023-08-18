#ifndef __RT_ISICG_PROCEDURAL_MATERIAL_HPP__
#define __RT_ISICG_PROCEDURAL_MATERIAL_HPP__

#include "base_material.hpp"
#include "brdfs/micro_brdf.hpp"
#include "texture.hpp"
#include "utils/noise.hpp"

enum NoiseType
{
	PERLIN_NOISE,
	GABOR_NOISE,
	LRP_NOISE
};

namespace RT_ISICG
{
	class ProceduralMaterial : public BaseMaterial
	{
	  public:
		ProceduralMaterial( const std::string & p_name,
							const float			p_scaleFactor,
							const float			p_brillance,
							const Vec2i &		p_resolution,
							const int			p_seed = 1,
							const NoiseType     p_noiseType = PERLIN_NOISE,
							const Vec3f &		p_F0   = VEC3F_ZERO )
			: BaseMaterial( p_name ), _scaleFactor( p_scaleFactor ), _microBRDF( p_brillance, p_F0 ), _f0( p_F0 ),
			  _textureSize( p_resolution ), _noiseType( p_noiseType )
		{
			// Generate first texture
			generateTexture( p_seed );
		}

		Vec3f shade( const Ray & p_ray, const HitRecord & p_hit, const LightSample & p_lightSample ) const override
		{
			// Get perturbed normal
			const Vec3f perturbNormal = _perturbNormal( Vec2f( p_hit._u, p_hit._v ), p_hit._normal );

			// Turn it, if it's not facing camera
			//perturbNormal = glm::dot( perturbNormal, p_ray.getDirection() ) < 0.f ? perturbNormal : -perturbNormal;

			// Evaluate BRDF with perturbed normal
			return _microBRDF.evaluate( p_lightSample._direction, p_ray.getDirection(), perturbNormal );
		}

		inline void generateTexture( const int p_seed = 1 )
		{
			_normalMap = new Texture( _textureSize.x, _textureSize.y, 3 );
			_normalMap->setScaleFactor( _scaleFactor );

			const auto noise	 = new float[ static_cast<int>( _textureSize.x * _textureSize.y ) ];
			const auto normalMap = new Vec3f[ static_cast<int>( _textureSize.x * _textureSize.y ) ];

			// Generate noise according to noise type chose by users
			switch (_noiseType)
			{
			case PERLIN_NOISE:
				Noise::perlinNoise( _textureSize.x, _textureSize.y, noise, p_seed );
				break;

			case GABOR_NOISE: 
				Noise::gaborNoise( _textureSize.x, _textureSize.y, noise, p_seed );
				break;

			case LRP_NOISE:
				Noise::lrpn( _textureSize.x, _textureSize.y, noise ,p_seed );
				break;

			default:
				break;
			}

			// Convert it to normal map
			Noise::convert2Normal( _textureSize, noise, normalMap );

			// Write data into a normal map texture
			for ( int i = 0; i < _textureSize.y; ++i )
			{
				for ( int j = 0; j < _textureSize.x; ++j )
					_normalMap->setPixel( i, j, Vec3f( normalMap[ i * _textureSize.x + j ] ) );
			}

			// Save texture, to see what it looks like
			_normalMap->saveJpg( "./results/texture.jpg" );
		}

		inline const Vec3f & getFlatColor() const override { return BLACK; }
		inline const Vec3f & getReflectivity() const override { return VEC3F_ZERO; }
		inline void			 setNoiseType( const NoiseType & p_noiseType ) { _noiseType = p_noiseType; }

	  private:
		inline const Vec3f & _perturbNormal( const Vec2f & p_uv, const Vec3f & p_geoNormal ) const
		{
			// Get perturbation from normal map
			Vec3f perturbation = _normalMap->bilinearFilter( p_uv.x, p_uv.y, true );
			perturbation = glm::normalize( perturbation * 2.f - 1.f );

			// Compute tangent space
			Vec3f newUpVector( 0.f, 0.f, -1.f );
			if ( p_geoNormal.y < -0.99f || p_geoNormal.y > 0.99f ) newUpVector = Vec3f( 1.f, 0.f, 0.f );

			const Vec3f pU = glm::normalize( glm::cross( newUpVector, p_geoNormal ) );
			const Vec3f pV = glm::normalize( glm::cross( pU, p_geoNormal ) );

			// Add perturbation to geometric normal
			const Vec3f perturbNormal
				= p_geoNormal + ( pU * perturbation.x + pV * perturbation.y + p_geoNormal * perturbation.z );

			return normalize( perturbNormal );
		}

		float _scaleFactor = 1.f;

		MicroBRDF _microBRDF;

		Vec3f	  _f0		   = VEC3F_ZERO;
		Vec2i	  _textureSize = VEC3F_ZERO;
		Texture * _normalMap   = nullptr;

		NoiseType _noiseType = PERLIN_NOISE;
	};
} // namespace RT_ISICG

#endif // ! __RT_ISICG_PROCEDURAL_MATERIAL_HPP__
