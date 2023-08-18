#ifndef __RT_ISICG_MICRO_BRDF__
#define __RT_ISICG_MICRO_BRDF__

#include "defines.hpp"

namespace RT_ISICG
{
	class MicroBRDF
	{
	  public:
		MicroBRDF( const float p_brillance, const Vec3f & p_F0 )
			: _F0( p_F0 ), _brillance( p_brillance ), _k( glm::pow( ( p_brillance + 1.f ), 2.f ) / 8.f ) {};

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

		inline Vec3f evaluate( const Vec3f & p_incident, const Vec3f & p_observation, const Vec3f & p_normal ) const
		{
			const float NdotO = glm::dot( p_observation, p_normal );
			const float NdotI = glm::dot( p_incident, p_normal );
			const Vec3f h = glm::normalize( p_observation + p_incident );

			// Evaluation only depend of fresnel factor
			// TO DO : Add masquage/ombrage with function above and add it to the evaluation
			return fresnel( h, p_observation ) * masquage( p_incident, -p_observation, p_normal ); 
		}

		inline const Vec3f & getKd() const { return VEC3F_ZERO; }

	  private:
		Vec3f _F0		 = VEC3F_ZERO;
		float _brillance = 0.f;
		float _k		 = 0.f;
	};
} // namespace RT_ISICG

#endif