#ifndef __RT_ISICG_NOISE_STATISTICS__
#define __RT_ISICG_NOISE_STATISTICS__

#include "defines.hpp"
#include "texture.hpp"

namespace RT_ISICG
{
	class NoiseStatistics
	{
	  public:
		NoiseStatistics() = default;

		NoiseStatistics( const Texture * p_textureArray, const int p_nbSample )
			: _textureArray( p_textureArray ), _nbSamples( p_nbSample )
		{}

		~NoiseStatistics() = default;

		Texture mean( const bool p_isSquared ) const;
		Texture variance() const;
		Texture std() const;
		Vec3f   textureMean( const Texture & p_texture ) const;
		

	  private:
		const Texture* _textureArray = nullptr;
		const int	_nbSamples = 0;
	};
} // namespace RT_ISICG

#endif
