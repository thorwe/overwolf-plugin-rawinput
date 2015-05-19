#include "RawInputAPI.h"

#include "RawHID.h"

namespace RawInput
{
	RawHID::RawHID( Handle handle )
		: RawDevice(handle),
		m_data()
	{
	}

	RawHID::~RawHID( void )
	{
	}

	void RawHID::Read( const RAWINPUT & ri )
	{
		m_data = ri.data.hid;
	}

	const RAWHID & RawHID::GetData( void ) const
	{
		return m_data;
	}
}