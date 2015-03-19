#ifndef RawHID_h__
#define RawHID_h__

#include "RawInputAPI.h"

#include "RawDevice.h"

namespace RawInput
{
	class RawHID : public RawDevice {
	public:
		typedef DeviceEvent<RawHID> Event;

		explicit RawHID( Handle handle );

		virtual ~RawHID( void );

		virtual void Read( const RAWINPUT & ri ) override;

		const RAWHID & GetData( void ) const;

	private:
		RAWHID m_data;
	};
}

#endif // RawHID_h__
