#include "virtual_point_light_integrator.hpp"
#include "objects/sphere.hpp"

#define ACCEPT_PROBA 0.4f

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
			//Additionner le direct lighting (soit les vraies lumières)
			const Vec3f directColor = _directLighting( p_scene, hitRecord, p_ray );
			//Et l'éclairage des VPLs
			const Vec3f vplColor	= _VPLLighting( p_scene, hitRecord, p_ray );
			//Et on moyenne des les deux éclairages
			return ( vplColor + directColor ) / 2.f;
		}
		return _backgroundColor;
	}

	float VirtualPointLightIntegrator::_computeAcceptanceProbability( const PointLight * p_light,
																	  const float		 p_totalLuminance ) const
	{
		// Phi(i)
		float pixelContribution = 0.f;

		for ( const HitRecord & hitRecord : _hitRecordSamples )
			pixelContribution += p_light->getPower() / glm::distance(p_light->getPosition(), hitRecord._point);

		pixelContribution /= _nbHitRecordSamples;

		// Acceptance Probability
		float temp = std::min( ( pixelContribution / p_totalLuminance ) + _epsilon, 1.f );
		return temp;
	}

	float VirtualPointLightIntegrator::_totalLuminance( const Scene & p_scene, const float p_tMin, const float p_tMax ) const
	{
		// Generate 50 VPLs randomly
		int				  nbVPL			 = 0;
		float			  totalLuminance = 0.f;
		const BaseLight * light			 = p_scene.getLights()[ 0 ];
		while ( nbVPL < 50 )
		{
			Ray reflectedRay = light->sampleLightRay(randomFloat(), randomFloat());

			HitRecord hitRecord;
			if ( p_scene.intersect( reflectedRay, p_tMin, p_tMax, hitRecord ) )
			{
				nbVPL++;

				// Compute luminance for _nbHitRecordSamples pixels from VPLs calculated
				const float power = light->getPower();
				for ( const HitRecord & hitRecord : _hitRecordSamples )
					totalLuminance += power / glm::pow(glm::distance( light->getPosition(), hitRecord._point ), 2.f);
			}
		}

		return totalLuminance / ( 50.f * _nbHitRecordSamples );
	}

	//Cette fonction provient du papier Instant Radiosity
	void VirtualPointLightIntegrator::sampleVPL( const Scene & p_scene, const float p_tMin, const float p_tMax )
	{
		//Calculer d'abord la luminosité totale
		const float totalLuminance = _totalLuminance( p_scene, p_tMin, p_tMax );

		//Pour toutes les lumières de la scène, on sample des VPLs
		for ( BaseLight * light : p_scene.getLights() )
		{
			int start = _nbVPL, end = _nbVPL;
			int reflections = 0;

			while ( end > 0 )
			{
				//Certaines VPLs seront réfléchies
				start = static_cast<int>( start * _probaDiffuse );

				for ( int i = start; i < end; ++i )
				{
					//Sampler la vraie lumière
					Ray reflectedRay = light->sampleLightRay( halton(2, i), halton(3, i) );

					HitRecord hitRecord;
					if ( !p_scene.intersect( reflectedRay, p_tMin, p_tMax, hitRecord ) ) continue;

					float		 nbParticles = static_cast<float>( _nbVPL );
					//Décrémenter la puissance des nouvelles VPLs pour éviter les splotchs
					float		 newPower	 = light->getPower() / glm::pow( hitRecord._distance, 2.f );
					PointLight * newVPL		 = new PointLight( hitRecord._point + hitRecord._normal * 2.f,
														   ( _nbVPL / nbParticles ) * ( light->getFlatColor() / PIf ),
														   newPower );

					//Si la VPL ainsi créé apporte assez de luminosité, on l'ajoute au lumières pour le rendu
					if ( _computeAcceptanceProbability( newVPL, totalLuminance ) > ACCEPT_PROBA )
						_VPLs.push_back( newVPL );

					//Et on sample de nouvelles VPLs réfléchie à partir de la précédente
					for ( int j = 1; j <= reflections; ++j )
					{
						// Sample hemisphere to not intersect with surface intersected
						const Vec3f position = hitRecord._point + hitRecord._normal * 2.f;

						//Sampler une hémisphère selon la normale de la surface
						const Vec3f direction = sampleHemisphere( hitRecord._normal, halton( 2 * j + 2, i ), halton( 2 * j + 3, i ) );
						reflectedRay = Ray( position, glm::normalize( direction ) );

						if ( !p_scene.intersect( reflectedRay, p_tMin, p_tMax, hitRecord ) ) break;

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
		//Cette méthode commenté permet de construire le light tree
		//Mais ce code ne fonctionne pas
		//_lightTree.build( &_VPLs );
		std::cout << "Nombre de VPL " << _VPLs.size() << std::endl;
	}

	//Fonction de Direct Lighting adapté aux VPLs
	Vec3f VirtualPointLightIntegrator::_VPLLighting( const Scene &	   p_scene,
													 const HitRecord & p_hitRecord,
													 const Ray &	   p_ray ) const
	{
		Vec3f		color	  = VEC3F_ZERO;
		//Pour éviter les problème de BRDF spéculaire, on recalcule le modèle de Lambert
		const Vec3f flatColor = p_hitRecord._object->getMaterial()->getFlatColor() * INV_PIf;
		const float N		  = static_cast<float>( _VPLs.size() );

		//Calculer l'éclairage pour toutes les VPLs de la scène
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
		//Normalement il faudrait diviser par le nombre de VPLs
		//Mais le résultat est très sombre
		return color;
	}

	//Réaliser l'éclairage avec la méthode des light cuts en coupant l'arbre
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

			//Calcul de l'erreur approché
			const float visibilityTerm = p_scene.intersectAny( shadowRay, 0.f, lightSample._distance );
			const float materialTerm   = ( ( diffuse.x + diffuse.y + diffuse.z ) / 3.f );
			const float geometricTerm  = 1.f / lightSample._distance;
			const float estimatedRadiance
				= visibilityTerm * cosTheta * materialTerm * geometricTerm * lightsIntensities;

			//Calculer l'erreur maximale
			const Vec3f centre			   = lightNode->_aabb.centroid();
			const Vec3f direction		   =  centre - p_hitRecord._point;
			const Ray	aabbRay			   = Ray( p_hitRecord._point, glm::normalize( direction ) );
			const float minDist			   = lightNode->_aabb.intersectDistance( aabbRay, 0.00001f, 1000000.f );
			bool		isIntersectign	   = lightNode->_aabb.intersect( aabbRay, 0.0001f, 1000000.f );
			const float boundGeometricTerm = 1.f / minDist;
			const float clusterErrorBound  = boundGeometricTerm * materialTerm * lightsIntensities;

			//Ratio de l'erreur pour un cluster
			const float errorRatio = clusterErrorBound / estimatedRadiance;
			
			//Si l'erreur est trop grande
			//Il faut recommencer pour les enfants de la node
			if (errorRatio > maxError) {
				nodesQueue.push(lightNode->_left);
				nodesQueue.push(lightNode->_right);
			}
			//Sinon on l'ajoute dans un vector de clusters
			else
				finalClusters.push_back( lightNode );

		}
	}
} // namespace RT_ISICG