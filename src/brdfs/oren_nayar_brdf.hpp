#ifndef __RT_ISICG_BRDF_OREN_NAYAR__
#define __RT_ISICG_BRDF_OREN_NAYAR__

#include "defines.hpp"

namespace RT_ISICG
{
	class OrenNayarBRDF
	{
	  public:
		OrenNayarBRDF( const Vec3f & p_kd, const float p_sigma ) : _kd( p_kd ), _sigma(p_sigma) {};

		inline Vec3f evaluate(const Vec3f & p_incident, const Vec3f & p_observation, const Vec3f & p_normal) const 
		{ 
			const Vec3f projIncident = p_incident    - p_normal;
			const Vec3f projObs		 = p_observation - p_normal;

			const float p_inTheta  = glm::acos( glm::dot( p_incident, p_normal ) );
			const float p_inPhi	   = glm::atan( projIncident.x / projIncident.y );
			const float p_outTheta = glm::acos( glm::dot( p_observation, p_normal ) );
			const float p_outPhi   = glm::atan( projObs.x / projObs.y );

			const float sigmaCarre = glm::pow( _sigma, 2.f );
			const float A	  = 1.f - ( 0.5f * ( sigmaCarre / ( sigmaCarre + 0.33f ) ) );
			const float B	  = 0.45f * ( sigmaCarre / ( sigmaCarre + 0.09f ) );
			const float alpha = glm::max( p_inTheta, p_outTheta );
			const float beta  = glm::min( p_inTheta, p_outTheta );
			return _kd * INV_PIf * ( A + ( B * glm::max(0.f, glm::cos(p_inPhi - p_outPhi) ) * glm::sin(alpha) * glm::tan(beta) ) );
		}

		inline const Vec3f & getKd() const { return _kd; }

	  private:
		Vec3f _kd = WHITE;
		float _sigma = 0.0f;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_BRDF_LAMBERT__
