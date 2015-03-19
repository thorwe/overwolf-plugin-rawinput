#include "RawInputAPI.h"

#include "RawMouse.h"

namespace RawInput
{
	RawMouse::RawMouse( Handle handle )
		: RawDevice(handle),
		m_data()
	{
	}

	RawMouse::~RawMouse( void )
	{
	}

	void RawMouse::Read( const RAWINPUT & ri )
	{
		m_data = ri.data.mouse;
	}

	const RAWMOUSE & RawMouse::GetData( void ) const
	{
		return m_data;
	}

	bool RawMouse::Button( unsigned short button ) const
	{
		return (m_data.usButtonFlags & button) != 0;
	}

	void RawMouse::GetPosXY( long * x, long * y ) const
	{
		*x = m_data.lLastX;

		*y = m_data.lLastY;
	}

	short RawMouse::GetWheelDelta( void ) const
	{
		return m_data.usButtonFlags & RI_MOUSE_WHEEL
			? m_data.usButtonData
			: 0;
	}
}
