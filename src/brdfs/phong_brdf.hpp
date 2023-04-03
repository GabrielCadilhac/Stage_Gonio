#ifndef __RT_ISICG_BRDF_PHONG__
#define __RT_ISICG_BRDF_PHONG__

#include "defines.hpp"

namespace RT_ISICG
{
	class PhongBRDF
	{
	  public:
		PhongBRDF( const Vec3f & p_kd, const float p_brillance ) : _kd( p_kd ), _ks( p_kd), _brillance( p_brillance ) {};

		inline Vec3f evaluate( const Vec3f & p_incident, const Vec3f & p_observation, const Vec3f & p_normal ) const
		{
			const float cosTheta  = glm::max( glm::dot( p_incident, p_normal ), 0.f );
			const Vec3f reflechie = glm::reflect( p_incident, p_normal );
			const float cosAlpha  = glm::max( glm::dot( p_observation, reflechie ), 0.f );
			return ( _kd * INV_PIf ) + ( _ks / cosTheta ) * glm::pow( cosAlpha, _brillance );
		}

		inline const Vec3f & getKd() const { return _kd; }

	  private:
		Vec3f _kd		 = WHITE;
		Vec3f _ks		 = WHITE;
		float _brillance = 0.0f;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_BRDF_PHONG__
