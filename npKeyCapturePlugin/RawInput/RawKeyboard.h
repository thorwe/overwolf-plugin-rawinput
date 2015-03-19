#ifndef RawKeyboard_h__
#define RawKeyboard_h__

#include "RawInputAPI.h"

#include "RawDevice.h"

namespace RawInput
{
	class RawKeyboard : public RawDevice {
	public:
		typedef DeviceEvent<RawKeyboard> Event;

		explicit RawKeyboard( Handle handle );

		virtual ~RawKeyboard( void );

		virtual void Read( const RAWINPUT & ri ) override;

		const RAWKEYBOARD & GetData( void ) const;

		/** \brief used to query if key was released.
		 *
		 * \param key VirtualKey code.
		 *
		 * \return true if key was released.
		 */
		bool KeyUp( unsigned short key ) const;

		/** \brief used to query if key was pressed.
		 *
		 * \param key VirtualKey code.
		 *
		 * \return true if key was pressed.
		 */
		bool KeyDown( unsigned short key ) const;

	private:
		RAWKEYBOARD m_data;
	};
}

#endif // RawKeyboard_h__
