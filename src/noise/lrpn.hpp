#ifndef __RT_ISICG_LRPN__
#define __RT_ISICG_LRPN__

#include "defines.hpp"

#define NB_OF_COSINES 3
#define LRPN_RESOLUTION 10.f

namespace RT_ISICG
{
	class LRPN
	{
	  public:
		// Generate Local Random Phase Noise
		static void generateNoise( const int p_width, const int p_height, float * p_output, const int p_seed );

	  private:
		static inline float _gaussian( const Vec2f & p_st, const float p_c, const Vec2f & p_mu, const Mat2f & p_sig )
		{
			const Vec2f p	 = p_st - p_mu;
			const float body = -0.5f * glm::dot( p, glm::inverse( p_sig ) * p );
			return p_c * glm::exp( body );
		}

		static Vec3f _noiseLRPN( const Vec2f & p_texCoords,
		                         Vec2f **      p_freqs,
		                         float **      p_amplitudes,
		                         float **      p_phase,
		                         const Vec2f *       p_positions );
	};
} // namespace RT_ISICG

#endif