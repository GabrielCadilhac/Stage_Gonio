#include "virtual_point_light_integrator.hpp"
#include "objects/sphere.hpp"

#define ACCEPT_PROBA 0.2f

namespace RT_ISICG
{
	Vec3f VirtualPointLightIntegrator::Li( const Scene & p_scene,
										   const Ray &	 p_ray,
										   const float	 p_tMin,
										   const float	 p_tMax ) const
	{
		HitRecord hitRecord;
		if ( p_scene.intersect( p_ray, p_tMin, p_tMax, hitRecord ) )
		{
			const Vec3f directColor = _directLighting( p_scene, hitRecord, p_ray );
			const Vec3f vplColor	= _VPLLighting( p_scene, hitRecord, p_ray );
			//return directColor;
			return ( vplColor + directColor ) / 2.f;
		}
		return _backgroundColor;
	}

	float VirtualPointLightIntegrator::_computeAcceptanceProbability( const PointLight * p_light,
																	  const float		 p_totalLuminance ) const
	{
		// Phi(i)
		float pixelContribution = 0.f;

		for ( const HitRecord * hitRecord : _hitRecordSamples )
			pixelContribution += p_light->getPower() / glm::distance( p_light->getPosition(), hitRecord->_point );

		pixelContribution /= _nbHitRecordSamples;

		// Acceptance Probability
		return std::min( ( pixelContribution / p_totalLuminance ) + _epsilon, 1.f );
	}

	float VirtualPointLightIntegrator::_totalLuminance( const Scene & p_scene ) const
	{
		// Generate 50 VPLs randomly
		int				  nbVPL			 = 0;
		float			  totalLuminance = 0.f;
		const BaseLight * light			 = p_scene.getLights()[ 0 ];
		while ( nbVPL < 50 )
		{
			Vec3f direction
				= glm::normalize( polarToCartesian( TWO_PIf * randomFloat(), glm::acos( 1.f - 2.f * randomFloat() ) ) );
			Ray reflectedRay( light->getPosition(), direction );

			HitRecord hitRecord;
			if ( p_scene.intersect( reflectedRay, 0.01f, 100.f, hitRecord ) )
			{
				nbVPL++;

				// Compute luminance for _nbHitRecordSamples pixels from VPLs calculated
				const float power = light->getPower();
				for ( const HitRecord * hitRecord : _hitRecordSamples )
					totalLuminance += power / glm::distance( light->getPosition(), hitRecord->_point );
			}
		}

		return totalLuminance / ( 50.f * _nbHitRecordSamples );
	}

	void VirtualPointLightIntegrator::sampleVPL( const Scene & p_scene, const float p_tMax )
	{
		const float totalLuminance = _totalLuminance( p_scene );

		for ( BaseLight * light : p_scene.getLights() )
		{
			int start = _nbVPL, end = _nbVPL;
			int reflections = 0;

			while ( end > 0 )
			{
				start = static_cast<int>( start * _probaDiffuse );

				for ( int i = start; i < end; ++i )
				{
					Ray reflectedRay = light->sampleLightRay( halton(2, i), halton(3, i) );

					HitRecord hitRecord;
					if ( !p_scene.intersect( reflectedRay, 0.01f, p_tMax, hitRecord ) ) continue;

					float		 nbParticles = static_cast<float>( _nbVPL );
					float		 newPower	 = light->getPower() / glm::pow( hitRecord._distance, 2.f );
					PointLight * newVPL		 = new PointLight( hitRecord._point + hitRecord._normal * 2.f,
														   ( _nbVPL / nbParticles ) * ( light->getFlatColor() / PIf ),
														   newPower );

					if ( _computeAcceptanceProbability( newVPL, totalLuminance ) > ACCEPT_PROBA )
						_VPLs.push_back( newVPL );

					for ( int j = 1; j <= reflections; ++j )
					{
						// Sample hemisphere to not intersect with surface intersected
						const Vec3f position = hitRecord._point + hitRecord._normal * 2.f;
						const Vec3f direction = sampleHemisphere( hitRecord._normal, halton( 2 * j + 2, i ), halton( 2 * j + 3, i ) );
						reflectedRay = Ray( position, glm::normalize( direction ) );

						if ( !p_scene.intersect( reflectedRay, 0.01f, p_tMax, hitRecord ) ) break;

						newPower *= 1.f / glm::pow( hitRecord._distance, 2.f );
						nbParticles *= _probaDiffuse;
						PointLight * newVPL = new PointLight(
							hitRecord._point + hitRecord._normal * 2.f, light->getFlatColor(), newPower );

						if ( _computeAcceptanceProbability( newVPL, totalLuminance ) > ACCEPT_PROBA )
							_VPLs.push_back( newVPL );
					}
				}
				reflections++;
				end = start;
			}
		}
		_lightTree.build( &_VPLs );
		std::cout << "Nombre de VPL " << _VPLs.size() << std::endl;
	}

	Vec3f VirtualPointLightIntegrator::_VPLLighting( const Scene &	   p_scene,
													 const HitRecord & p_hitRecord,
													 const Ray &	   p_ray ) const
	{
		Vec3f		color	  = VEC3F_ZERO;
		const Vec3f flatColor = p_hitRecord._object->getMaterial()->getFlatColor() * INV_PIf;
		const float N		  = static_cast<float>( _VPLs.size() );

		for ( PointLight * light : _VPLs )
		{
			const LightSample lightSample = light->sample( p_hitRecord._point );
			Ray				  shadowRay( p_hitRecord._point, lightSample._direction );
			shadowRay.offset( p_hitRecord._normal );

			if ( !p_scene.intersectAny( shadowRay, 0.f, lightSample._distance ) )
			{
				const float cosTheta = std::max( glm::dot( p_hitRecord._normal, lightSample._direction ), 0.f );
				color += flatColor * lightSample._radiance * cosTheta;
			}
		}
		return color;
	}

	void VirtualPointLightIntegrator::lightCutsLighting( const Scene &	 p_scene,
														   const HitRecord & p_hitRecord,
														   const Ray &		 p_ray ) const
	{
		const float			  maxError = 0.02f;
		std::queue<LightNode*> nodesQueue;
		nodesQueue.push( _lightTree._root );
		std::vector<LightNode*>	finalClusters;
		while ( !nodesQueue.empty() ) 
		{
			LightNode * lightNode = nodesQueue.front();
			nodesQueue.pop();
			BaseLight * light = lightNode->_pointLight;
			LightSample lightSample = light->sample( p_hitRecord._point );
			Ray			shadowRay( p_hitRecord._point, lightSample._direction );
			const float cosTheta	   = std::max( glm::dot( p_hitRecord._normal, lightSample._direction ), 0.f );
			const Vec3f diffuse	 = p_hitRecord._object->getMaterial()->getFlatColor();
			const float lightsIntensities = lightNode->_sumIntensities();

			const float visibilityTerm = p_scene.intersectAny( shadowRay, 0.f, lightSample._distance );
			const float materialTerm   = ( ( diffuse.x + diffuse.y + diffuse.z ) / 3.f );
			const float geometricTerm  = 1.f / lightSample._distance;
			const float estimatedRadiance
				= visibilityTerm * cosTheta * materialTerm * geometricTerm * lightsIntensities;

			//Calculer un rayon
			const Vec3f centre			   = lightNode->_aabb.centroid();
			const Vec3f direction		   =  centre - p_hitRecord._point;
			const Ray	aabbRay			   = Ray( p_hitRecord._point, glm::normalize( direction ) );
			const float minDist			   = lightNode->_aabb.intersectDistance( aabbRay, 0.00001f, 1000000.f );
			bool		isIntersectign	   = lightNode->_aabb.intersect( aabbRay, 0.0001f, 1000000.f );
			const float boundGeometricTerm = 1.f / minDist;
			const float clusterErrorBound  = boundGeometricTerm * materialTerm * lightsIntensities;

			const float errorRatio = clusterErrorBound / estimatedRadiance;
			
			if (errorRatio > maxError) {
				nodesQueue.push(lightNode->_left);
				nodesQueue.push(lightNode->_right);
			}
			else
				finalClusters.push_back( lightNode );
		}
	}
} // namespace RT_ISICG