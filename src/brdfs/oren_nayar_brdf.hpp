#ifndef __RT_ISICG_BRDF_OREN_NAYAR__
#define __RT_ISICG_BRDF_OREN_NAYAR__

#include "defines.hpp"
#include <glm/gtx/polar_coordinates.hpp>

namespace RT_ISICG
{
	class OrenNayarBRDF
	{
	  public:
		OrenNayarBRDF( const Vec3f & p_kd, const float p_sigma ) : _kd( p_kd )
		{
			const float sigmaCarre = glm::pow( p_sigma, 2.f );
			_A					   = 1.f - ( 0.5f * ( sigmaCarre / ( sigmaCarre + 0.33f ) ) );
			_B					   = 0.45f * ( sigmaCarre / ( sigmaCarre + 0.09f ) );
		};

		inline Vec3f evaluate( const Vec3f & p_incident, const Vec3f & p_observation, const Vec3f & p_normal ) const
		{
			const Vec3f obesvation = -p_observation;

            const float cosThetaO = glm::dot( p_normal, obesvation );
			const float cosThetaI = glm::dot( p_normal, p_incident );

			const float outTheta = glm::acos( cosThetaO );
			const float inTheta  = glm::acos( cosThetaI );

			const Vec3f woPhi = glm::normalize( obesvation - p_normal * cosThetaO );
			const Vec3f wiPhi = glm::normalize( p_incident - p_normal * cosThetaI );
			const float cosPhiIO = glm::max(glm::dot( woPhi, wiPhi ), 0.f);

			const float alpha = glm::max( inTheta, outTheta );
			const float beta  = glm::min( inTheta, outTheta );
			return _kd * INV_PIf * ( _A + ( _B * cosPhiIO * glm::sin( alpha ) * glm::tan( beta ) ) );
		}

		inline const Vec3f & getKd() const { return _kd; }

	  private:
		Vec3f _kd = WHITE;
		float _A;
		float _B;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_BRDF_LAMBERT__
