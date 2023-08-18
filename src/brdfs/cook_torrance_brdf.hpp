#ifndef __RT_ISICG_COOK_TORRANCE_LAMBERT__
#define __RT_ISICG_COOK_TORRANCE_LAMBERT__

#include "defines.hpp"

namespace RT_ISICG
{
	class CookTorranceBRDF
	{
	  public:
		CookTorranceBRDF( const Vec3f & p_kd, const float p_brillance, const Vec3f & p_F0 )
			: _kd( p_kd ), _alpha( glm::pow( p_brillance, 2.f ) ),
			  _F0( p_F0 ),
			  _k( glm::pow( ( p_brillance + 1.f ), 2.f ) / 8.f ) {}

		inline float distribNormalGGX( const Vec3f & p_h, const Vec3f & p_normal ) const
		{
			const float alphaSquared = glm::pow( _alpha, 2.f );
			const float NdotH	   = glm::pow( glm::dot( p_h, p_normal ), 2.f );
			return INV_PIf * ( alphaSquared / glm::pow( NdotH * ( alphaSquared - 1.f ) + 1.f, 2.f ) );
		}

		inline float distribNormalBeckmann( const Vec3f & p_h, const Vec3f & p_normal ) const
		{
			const float alphaSquared = glm::pow( _alpha, 2.f );
			const float NdotH	   = glm::dot( p_h, p_normal );
			const float theta	   = glm::acos( NdotH );
			const float p
				= INV_PIf * ( 1.f / alphaSquared ) * glm::exp( -glm::pow( glm::tan( theta ), 2.f ) / alphaSquared );
			return p / glm::pow( NdotH, 4.f );
		}

		inline float distribNormalBeckmannDecentre( const Vec3f & p_perturbation,
													const Vec3f & p_geoNormal,
													const Vec3f & p_microNormal ) const
		{
			const float geoDotMicro = glm::dot( p_geoNormal, p_microNormal );

			const Vec2f p( -p_perturbation.x / p_perturbation.z, -p_perturbation.y / p_perturbation.z );
			const Vec2f m( -p_microNormal.x / p_microNormal.z, -p_microNormal.y / p_microNormal.z );
			constexpr Vec2f alpha( 0.25f, 0.25f );
			constexpr float c = 0.f;

			const float x = glm::pow( m.x - p.x, 2.f ) / ( alpha.x * alpha.x );
			const float y = glm::pow( m.y - p.y, 2.f ) / ( alpha.y * alpha.y );
			const float z = ( 2.f * c * ( m.x - p.x ) * ( m.y - p.y ) ) / ( alpha.x * alpha.y );

			const float p22 = ( 1.f / ( PIf * alpha.x * alpha.y * glm::sqrt( 1.f - c * c ) ) )
							  * glm::exp( ( -1.f / ( 1.f - c * c ) ) * ( x + y - z ) );

			return p22 / glm::pow( geoDotMicro, 4.f );
		}

		inline float G1( const float p_x ) const { return p_x / ( p_x * ( 1.f - _k ) + _k ); }

		inline float masquage( const Vec3f & p_incident, const Vec3f & p_observation, const Vec3f & p_normal ) const
		{
			const float NdotO = glm::dot( p_normal, p_observation );
			const float NdotI = glm::dot( p_normal, p_incident );
			return G1( NdotI ) * G1( NdotO );
		}

		inline Vec3f fresnel( const Vec3f & p_h, const Vec3f & p_observation ) const
		{
			return _F0 + ( 1.f - _F0 ) * glm::pow( 1.f - glm::dot( p_h, p_observation ), 5.f );
		}

		inline Vec3f evaluateMicroFacette( const Vec3f & p_incident,
										   const Vec3f & p_observation,
										   const Vec3f & p_perturbNormal,
										   const Vec3f & p_geoNormal,
										   const Vec3f & p_perturbation ) const
		{
			const float PdotG = glm::dot( p_perturbNormal, p_geoNormal );
			const float PdotO = glm::max( glm::dot( p_observation, p_perturbNormal ), FLT_EPSILON );
			const float GdotI = glm::dot( p_incident, p_geoNormal );
			const Vec3f h	  = glm::normalize( p_observation + p_incident );
			return ( PdotG / PdotO )
				   * ( fresnel( h, p_observation )
					   * distribNormalBeckmannDecentre( p_perturbation, p_geoNormal, p_perturbNormal )
					   * masquage( p_incident, p_observation, p_geoNormal ) )
				   / ( 4.f * GdotI );
		}

		inline Vec3f evaluate( const Vec3f & p_incident, const Vec3f & p_observation, const Vec3f & p_normal ) const
		{
			const float NdotO = glm::dot( p_observation, p_normal );
			const float NdotI = glm::dot( p_incident, p_normal );
			const Vec3f h	  = glm::normalize( p_observation + p_incident );
			return ( fresnel( h, p_observation ) * distribNormalGGX( h, p_normal )
					 * masquage( p_incident, p_observation, p_normal ) )
				   / ( 4.f * NdotO * NdotI );
		}

		inline const Vec3f & getKd() const { return _kd; }

	  private:
		Vec3f _kd	 = WHITE;
		float _alpha = 1.f;
		Vec3f _F0	 = VEC3F_ZERO;
		float _k	 = 0.f;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_COOK_TORRANCE_LAMBERT__
