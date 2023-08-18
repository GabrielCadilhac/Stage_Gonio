#ifndef __RT_ISICG_MANAGER_MOVEMENT__
#define __RT_ISICG_MANAGER_MOVEMENT__

//std
#include <map>
#include <functional>
#include <memory>
#include <typeindex>
#include <string>

namespace RT_ISICG
{
	class MovementEmitter
	{
	  private:
		class BaseListener
		{
		  public:
			explicit BaseListener()							 = default;
			BaseListener( const BaseListener & )			 = delete;
			BaseListener & operator=( const BaseListener & ) = delete;
			virtual ~BaseListener()							 = default;
		};

		template<typename ValueType>
		class Listener : public BaseListener
		{
		  public:
			explicit Listener( const std::function<void( ValueType )> & callback ) { this->_callback = callback; }
			Listener( const Listener & )			 = delete;
			Listener & operator=( const Listener & ) = delete;
			virtual ~Listener()						 = default;

			void emit( const ValueType value ) const
			{
				try
				{
					_callback( value );
				}
				catch ( ... )
				{
				}
			}

		  private:
			std::function<void( ValueType )> _callback;
		};

	  public:
		// Call previously defined function, defined by a string name
		template<typename ListenerType, typename ValueType>
		void emit( const std::string & p_emitName, ValueType type )
		{
			const auto it = _listeners.find( p_emitName );
			if ( it != _listeners.end() ) 
				static_cast<Listener<ValueType> *>( it->second.get() )->emit( type );
		}

		// Add function to be called when .emit()
		template<typename ListenerType, typename ValueType>
		MovementEmitter & on( const std::string & p_name, const std::function<void( ValueType )> & callback )
		{
			_listeners[ p_name ] = std::unique_ptr<BaseListener>( new Listener<ValueType>( callback ) );
			return *this;
		}

	  private:
		std::map<std::string, std::unique_ptr<BaseListener>> _listeners;
	};

} // namespace RT_ISICG

#endif // !__RT_ISICG_MANAGER_MOVEMENT__
