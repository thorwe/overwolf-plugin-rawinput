#ifndef RawDevice_h__
#define RawDevice_h__

#include "RawInputAPI.h"

#include <functional>
#include <memory>

namespace RawInput
{
	class RawDevice {
		RawDevice( const RawDevice & );

		const RawDevice & operator=( const RawDevice & );

	public:
		typedef HANDLE Handle;

		typedef std::shared_ptr<RawDevice> Ptr;

		//http://www.microsoft.com/whdc/archive/HID_HWID.mspx
		enum usage_page {
			HID_USAGE_PAGE				= 0x01
		};

		enum usage_id {
			HID_DEVICE_SYSTEM_POINTER	= 0x01,
			HID_DEVICE_SYSTEM_MOUSE		= 0x02,
			HID_DEVICE_SYSTEM_JOYSTICK	= 0x04,
			HID_DEVICE_SYSTEM_GAMEPAD	= 0x05,
			HID_DEVICE_SYSTEM_KEYBOARD	= 0x06,
			HID_DEVICE_SYSTEM_KEYPAD	= 0x07,
			HID_DEVICE_SYSTEM_CONTROL	= 0x08,
		};

		template <class DevType>
		struct DeviceEvent : public std::function<void(const DevType &)> {
			DeviceEvent( const typename DeviceEvent::function && callable )
				: function<void(const DevType &)>(callable) {}
		};

		explicit RawDevice( Handle handle ) : m_handle(handle) {}

		virtual ~RawDevice( void ) {}

		virtual void Read( const RAWINPUT & ri ) = 0;

		Handle GetHandle( void ) const { return m_handle; }

	private:
		Handle m_handle;
	};
}

#endif // RawDevice_h__
