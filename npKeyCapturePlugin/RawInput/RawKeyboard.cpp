#include "RawInputAPI.h"

#include "RawKeyboard.h"

namespace RawInput
{
	RawKeyboard::RawKeyboard( Handle handle )
		: RawDevice(handle),
		m_data()
	{
	}

	RawKeyboard::~RawKeyboard( void )
	{
	}

	void RawKeyboard::Read( const RAWINPUT & ri )
	{
		m_data = ri.data.keyboard;
	}

	const RAWKEYBOARD & RawKeyboard::GetData( void ) const
	{
		return m_data;
	}

	bool RawKeyboard::KeyUp( unsigned short key ) const
	{
		return m_data.VKey == key && m_data.Flags & RI_KEY_BREAK;
	}

	bool RawKeyboard::KeyDown( unsigned short key ) const
	{
		return m_data.VKey == key && !(m_data.Flags & RI_KEY_MAKE);
	}
}
