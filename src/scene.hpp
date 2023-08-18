#ifndef __RT_ISICG_SCENE__
#define __RT_ISICG_SCENE__

#include "defines.hpp"
#include "lights/base_light.hpp"
#include "objects/base_object.hpp"
#include <exception>
#include <map>
#include <vector>
#include "materials/procedural_material.hpp"

namespace RT_ISICG
{
	using ObjectMap		  = std::map<const std::string, BaseObject *>;
	using ObjectMapPair	  = ObjectMap::value_type;
	using MaterialMap	  = std::map<const std::string, BaseMaterial *>;
	using MaterialMapPair = MaterialMap::value_type;
	using LightList		  = std::vector<BaseLight *>;

	class Scene
	{
	  public:
		Scene();
		~Scene();

		// Hard coded initialization.
		void init();
		void init_stage( const Vec2i & p_textureSize, const Vec3f & p_lightPosition, const int p_seed = 1 );
		void init_test();

		void inline setTextureScale( const float p_scale ) { _textureScale = p_scale; }

		// Initialization from file.
		void init( const std::string & p_path ) { throw std::exception( "Not implemented !" ); }

		void loadFileTriangleMesh( const std::string & p_name, const std::string & p_path );

		const LightList & getLights() const { return _lightList; }

		// Check for nearest intersection between p_tMin and p_tMax : if found fill p_hitRecord.
		bool intersect( const Ray & p_ray, const float p_tMin, const float p_tMax, HitRecord & p_hitRecord ) const;
		bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const;

		inline void moveLight( const Vec3f & p_newPosition, const int p_lightIndex ) const
		{
			if ( _lightList.size() < p_lightIndex )
				std::cerr << "Light Index is greater than lights list size !" << std::endl;

			_lightList[ p_lightIndex ]->setPosition( p_newPosition );
		}

		inline void textureSample(const std::string & p_materialName, const int p_seed = 1)
		{
			BaseMaterial * baseMaterial = _materialMap[ p_materialName ];
			auto *         material     = dynamic_cast<ProceduralMaterial *>( baseMaterial );

			if ( material == nullptr )
				std::cout << "The material name doesn't corresponds to a ProceduralMaterial" << std::endl;

			material->generateTexture( p_seed );
		}

		inline Vec3f getLightPos( const int p_lightIndex ) const { return _lightList[ p_lightIndex ]->getPosition(); }

		void _addObject( BaseObject * p_object );
		void _attachMaterialToObject( const std::string & p_materialName, const std::string & p_objectName );

	  private:
		void _addMaterial( BaseMaterial * p_material );
		void _addLight( BaseLight * p_light );

	  private:
		ObjectMap	_objectMap;
		MaterialMap _materialMap;
		LightList	_lightList;

		float _textureScale = 1.f;
	};
} // namespace RT_ISICG

#endif // __RT_ISICG_SCENE__
