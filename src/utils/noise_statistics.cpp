#include "noise_statistics.hpp"

#include "utils/convert.hpp"

namespace RT_ISICG
{
	class Vec3d
	{
	public:
		Vec3d() = default;

		Vec3d(double p_scalar)
		{
			x = p_scalar;
			y = p_scalar;
			z = p_scalar;
		}

		Vec3d( double p_x, double p_y, double p_z )
		{
			x = p_x;
			y = p_y;
			z = p_z;
		}

		Vec3d(const Vec3f & p_vector)
		{
			x = static_cast<double>(p_vector.x);
			y = static_cast<double>(p_vector.y);
			z = static_cast<double>(p_vector.z);
		}

		void operator+=(const Vec3d & p_vector)
		{
			x += p_vector.x;
			y += p_vector.y;
			z += p_vector.z;
		}

		Vec3f castFloat() const
		{
			const auto xF = static_cast<float>(x);
			const auto yF = static_cast<float>(y);
			const auto zF = static_cast<float>(z);
			return { xF, yF, zF };
		}

		double x, y, z;
	};

	Texture NoiseStatistics::mean( const bool p_isSquared = false ) const
	{
		const int textWidth	 = _textureArray[ 0 ].getWidth();
		const int textHeight = _textureArray[ 0 ].getHeight();

		const auto meanArray = new Vec3d[ 10 ][ 10 ];

		const Texture meanTexture { textWidth, textHeight };
		for ( int i = 0; i < textHeight; i++ )
		{
			for ( int j = 0; j < textWidth; j++ )
				meanArray[ i ][ j ] = Vec3d( 0.0 );
		}

		for ( int i = 0; i < _nbSamples; ++i )
		{
			const Texture & texture = _textureArray[ i ];
			for ( int y = 0; y < textHeight; y++ )
			{
				for ( int x = 0; x < textWidth; x++ )
				{
					//const Vec3f meanColor = meanTexture.getColorFromPixelCoord( { y, x } );

					Vec3f textureColor = texture.getColorFromPixelCoord( { y, x } );

					if ( p_isSquared ) textureColor = textureColor * textureColor;

					textureColor /= static_cast<float>( _nbSamples );

					//meanTexture.setPixel( x, y, (textureColor + meanColor) / 255.f );
					meanArray[ x ][ y ] += Vec3d( textureColor );
				}
			}
		}

		for ( int i = 0; i < textHeight; i++ )
		{
			for ( int j = 0; j < textWidth; j++ )
				meanTexture.setPixel( i, j, meanArray[ i ][ j ].castFloat() / 255.f );
		}

		return meanTexture;
	}

	Texture NoiseStatistics::variance() const
	{
		const int textWidth	 = _textureArray[ 0 ].getWidth();
		const int textHeight = _textureArray[ 0 ].getHeight();

		const Texture meanTexture		   = mean( false );
		const Texture meanOfSquaredTexture = mean( true );
		Texture		  finalTexture { textWidth, textHeight };

		for ( int i = 0; i < textHeight; i++ )
		{
			for ( int j = 0; j < textWidth; j++ )
			{
				const Vec3f meanOfSquare = meanOfSquaredTexture.getColorFromPixelCoord( { i, j } ) / 255.f;
				const Vec3f squaredMean	 = meanTexture.getColorFromPixelCoord( { i, j } ) / 255.f;

				const float colorMeanOfSquare = ( meanOfSquare.x + meanOfSquare.y + meanOfSquare.z ) / 3.f;
				const float colorSquaredMean  = ( squaredMean.x + squaredMean.y + squaredMean.z ) / 3.f;

				const float difference = glm::abs( colorSquaredMean - colorMeanOfSquare );

				finalTexture.setPixel( j, i, Vec3f(glm::clamp( difference, 0.f, 1.f ) ) );
			}
		}

		return finalTexture;
	}

	Texture NoiseStatistics::std() const
	{
		const Texture varianceTexture = variance();

		const int	  textWidth	 = _textureArray[ 0 ].getWidth();
		const int	  textHeight = _textureArray[ 0 ].getHeight();
		const Texture finalImage { textWidth, textHeight };

		for ( int i = 0; i < textHeight; i++ )
		{
			for ( int j = 0; j < textWidth; j++ )
			{
				const Vec3f varianceColor = varianceTexture.getColorFromPixelCoord( { i, j } ) / 255.f;
				const float colorBW		  = ( varianceColor.x + varianceColor.y + varianceColor.z ) / 3.f;
				const Vec3f stdColor	  = Vec3f{ glm::sqrt( colorBW ) };
				finalImage.setPixel( j, i, stdColor );
			} 
		}

		return finalImage;
	}

	Vec3f NoiseStatistics::textureMean( const Texture & p_texture ) const
	{
		const float coeff = 1.f / static_cast<float>( p_texture.getWidth() * p_texture.getHeight() );
		Vec3f mean = VEC3F_ZERO;
		for ( int i = 0; i < p_texture.getHeight(); ++i )
		{
			for ( int j = 0; j < p_texture.getWidth(); ++j )
				mean += p_texture.getColorFromPixelCoord( { i, j } ) * coeff;
		}
		return mean;
	}
} // namespace RT_ISICG