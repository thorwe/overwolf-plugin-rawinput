#ifndef RawMouse_h__
#define RawMouse_h__

#include "RawInputAPI.h"

#include "RawDevice.h"

namespace RawInput
{
	class RawMouse : public RawDevice {
	public:
		typedef DeviceEvent<RawMouse> Event;

		enum mouse_flags {
			BUTTON_1_DOWN	= RI_MOUSE_BUTTON_1_DOWN,
			BUTTON_1_UP		= RI_MOUSE_BUTTON_1_UP,

			BUTTON_2_DOWN	= RI_MOUSE_BUTTON_2_DOWN,
			BUTTON_2_UP		= RI_MOUSE_BUTTON_2_UP,

			BUTTON_3_DOWN	= RI_MOUSE_BUTTON_3_DOWN,
			BUTTON_3_UP		= RI_MOUSE_BUTTON_3_UP,

			BUTTON_4_DOWN	= RI_MOUSE_BUTTON_4_DOWN,
			BUTTON_4_UP		= RI_MOUSE_BUTTON_4_UP,

			BUTTON_5_DOWN	= RI_MOUSE_BUTTON_5_DOWN,
			BUTTON_5_UP		= RI_MOUSE_BUTTON_5_UP,

			WHEEL			= RI_MOUSE_WHEEL
		};

		explicit RawMouse( Handle handle );

		virtual ~RawMouse( void );

		/** \brief Reads the Windows structure.
		 *
		 * \param button one of Mouse::mouse_flags enum.
		 *
		 * \return true if button flag is true.
		 */
		virtual void Read( const RAWINPUT & ri ) override;
		
		/** \brief Used to read the raw mouse data.
		 *
		 * \param button one of Mouse::mouse_flags enum.
		 *
		 * \return true if button flag is true.
		 */
		const RAWMOUSE & GetData( void ) const;

		/** \brief Used to query button state.
		 *
		 * \param button one of Mouse::mouse_flags enum.
		 *
		 * \return true if button flag is true.
		 */
		bool Button( unsigned short button ) const;

		/** \brief Gets mouse (x, y) coordinates.
		 *
		 * \param x returns mouse last position X.
		 * \param y returns mouse last position Y.
		 */
		void GetPosXY( long * x, long * y ) const;

		/** \brief Gets wheel movement delta.
		 *
		 * \return a value representing the wheel's movement delta.
		 */
		short GetWheelDelta( void ) const;

	private:
		RAWMOUSE m_data;
	};
}

#endif // RawMouse_h__
