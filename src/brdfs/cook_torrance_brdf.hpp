#ifndef __RT_ISICG_COOK_TORRANCE_LAMBERT__
#define __RT_ISICG_COOK_TORRANCE_LAMBERT__

#include "defines.hpp"

namespace RT_ISICG
{
	class CookTorranceBRDF
	{
	  public:
		CookTorranceBRDF( const Vec3f & p_kd, const float p_brillance, const Vec3f & p_F0 )
			: _kd( p_kd ), _k( glm::pow( ( p_brillance + 1.f ), 2.f ) / 8.f ), _alpha( glm::pow( p_brillance, 2.f ) ),
			  _F0( p_F0 ) {};

		inline float distribNormal( const Vec3f & p_h, const Vec3f & p_normal ) const
		{
			const float alphaCarre = glm::pow( _alpha, 2.f );
			const float NdotH	   = glm::pow( glm::dot( p_h, p_normal ), 2.f );
			return INV_PIf * ( alphaCarre / glm::pow( NdotH * ( alphaCarre - 1.f ) + 1.f, 2.f ) );
		}

		inline float G1( const float p_x ) const
		{
			return p_x / ( p_x * ( 1.f - _k ) + _k );
		}

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

		inline Vec3f evaluate( const Vec3f & p_incident, const Vec3f & p_observation, const Vec3f & p_normal ) const
		{
			const float NdotO = glm::dot( p_observation, p_normal );
			const float NdotI =  glm::dot( p_incident, p_normal );
			const Vec3f h     = glm::normalize( p_observation + p_incident ); // / glm::length(p_observation + p_incident);
			return ( fresnel( h, p_observation) * distribNormal(h,p_normal) * masquage(p_incident, p_observation, p_normal))
				   / ( 4.f * NdotO * NdotI );
		}

		inline const Vec3f & getKd() const { return _kd; }

	  private:
		Vec3f _kd		 = WHITE;
		float _alpha	 = 1.f;
		Vec3f _F0		 = VEC3F_ZERO;
		float _k		 = 0.f;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_COOK_TORRANCE_LAMBERT__
