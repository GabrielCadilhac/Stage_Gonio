#ifndef __RT_ISICG_DATA_SAVER__
#define __RT_ISICG_DATA_SAVER__

#include "defines.hpp"
#include "nlohmann/json.hpp"
#include "texture.hpp"

//utils
#include "utils/convert.hpp"

// std
#include <fstream>
#include <map>
#include <vector>

using json = nlohmann::json;

namespace RT_ISICG
{
	class DataSaver
	{
	  public:
		DataSaver()	 = default;
		~DataSaver() = default;

		// Save std::map with string keys
		inline void addMapData( const std::string & p_dataName, const std::map<std::string, Vec3f> & p_data )
		{
			for ( const auto & data : p_data )
				_jsonFile[ p_dataName ][ data.first ] = vec3ToString( data.second );
		}

		// Save a vector of Vec3f
		inline void addVectorData( const std::string &		  p_dataName,
								   const std::vector<Vec3f> & p_movement,
								   const std::string &		  p_secondName = ""
		)
		{
			if (p_secondName.empty())
				_jsonFile[ p_dataName ] = json::array();
			else
				_jsonFile[ p_dataName ][ p_secondName ] = json::array();

			for ( const auto & movement : p_movement )
				if (p_secondName.empty())
					_jsonFile[ p_dataName ].push_back( vec3ToString( movement ) );
				else
					_jsonFile[ p_dataName ][ p_secondName ].push_back( vec3ToString( movement ) );
		}

		// Save a single float
		inline void addFloat( const std::string & p_dataName, const float p_value )
		{
			_jsonFile[ p_dataName ][ "value" ] = p_value;
		}

		// Save a map of Vec3f arrays
		inline void addMapArrays( const std::string &			   p_dataName,
								  const std::map<float, Vec3f *> & p_datas,
								  const int						   p_arraySize )
		{
			for ( const std::pair<float, Vec3f *> data : p_datas )
			{
				_jsonFile[ p_dataName ][ std::to_string( data.first ) ] = json::array();

				for ( int i = 0; i < p_arraySize; ++i )
				{
					const Vec3f color = data.second[ i ];
					auto &		outer = _jsonFile[ p_dataName ][ std::to_string( data.first ) ];

					for ( int k = 0; k < 3; ++k )
						outer[ i ].push_back( static_cast<int>( color[ k ] ) );
				}
			}
		}

		// Save an image
		inline void addTexture( const std::string & p_dataName, const Texture & p_texture, const float p_angle)
		{
			_jsonFile[ p_dataName ][ std::to_string( p_angle ) ] = json::array();

			for ( int i = 0; i < p_texture.getHeight(); i++ )
			{
				auto & line = _jsonFile[ p_dataName ][ std::to_string( p_angle ) ];
				for ( int j = 0; j < p_texture.getWidth(); j++ )
				{
					const Vec3f color = p_texture.getColorFromPixelCoord( { i, j } );

					auto & colorArray = line[ i ];

					for ( int k = 0; k < 3; k++ )
						colorArray[ j ].push_back( static_cast<int>( color[ k ] ) );
				}
			}
		}

		inline void addTexture( const std::string & p_dataName, const Texture & p_texture, const Vec3f & p_value)
		{
			_jsonFile[ p_dataName ][ vec3ToString( p_value ) ] = json::array();

			for ( int i = 0; i < p_texture.getHeight(); i++ )
			{
				auto & line = _jsonFile[ p_dataName ][ vec3ToString( p_value ) ];
				for ( int j = 0; j < p_texture.getWidth(); j++ )
				{
					const Vec3f color = p_texture.getColorFromPixelCoord( { i, j } );

					auto &		colorArray = line[ i ];

					for ( int k = 0; k < 3; k++ )
						colorArray[ j ].push_back( static_cast<int>( color[ k ] ) );
				}
			}
		}

		inline void addString( const std::string & p_dataName, const std::string & p_value )
		{
			_jsonFile[ p_dataName ] = p_value;
		}

		// Take all data added and save them into .json file at p_location
		inline void saveData( const std::string & p_location ) const
		{
			std::ofstream file( p_location );
			file << _jsonFile;
		}

	  private:
		nlohmann::ordered_json _jsonFile;
	};
} // namespace RT_ISICG

#endif // !__RT_ISICG_DATA_SAVER__
