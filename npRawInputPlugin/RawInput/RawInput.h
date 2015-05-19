#ifndef RawInput_h__
#define RawInput_h__

#include "RawInputAPI.h"

#include "RawMouse.h"
#include "RawKeyboard.h"
#include "RawHID.h"

#include <algorithm>
#include <exception>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#ifndef NDEBUG
#include <iostream>
#include <fstream>
#endif

namespace RawInput
{
	class RawInputException : public std::runtime_error {
	public:
		explicit RawInputException( const std::string & msg, DWORD hr = ::GetLastError() )
			: runtime_error(msg.data()),
			m_hresult(hr)
		{
		}

		void Show( void )
		{
			/*std::wstringstream ss;
			
			ss << this->what() << TEXT(" has thrown an exception.\n\nCode: ") << m_hresult;

			::MessageBox(
				nullptr,
				ss.str().c_str(),
				TEXT("RawInput"),
				MB_ICONERROR);*/
		}

	private:
		RawInputException operator=(const RawInputException &);

		const DWORD m_hresult;
	};

	/** \brief This policy class defines unbuffered updates.
	 */
	class Unbuffered {
		protected:
		/** \brief Unbuffered Update implementation.
		 */
		RAWINPUT Update( const HRAWINPUT hrawinput )
		{
			UINT pcbSize;

			// If pData is NULL and the function is successful, the return value is 0.
			if (::GetRawInputData(
				hrawinput,
				RID_INPUT,
				nullptr,
				&pcbSize,
				sizeof(RAWINPUTHEADER)) != 0) throw RawInputException(__FUNCTION__" [Unable to get input data]");

			RAWINPUT rData;

			// If pData is not NULL and the function is successful, the return value is the number of bytes copied into pData.
			// Returns -1 on fail. Returns input data in rData.
			if (::GetRawInputData(
				hrawinput,
				RID_INPUT,
				&rData,
				&pcbSize,
				sizeof(RAWINPUTHEADER)) == -1) throw RawInputException(__FUNCTION__" [Unable to get input data]");

			return rData;
		}
	};

	/** \brief [WIP] This policy class defines buffered updates.
	 */
	class Buffered {
		protected:
		/** \brief Buffered Update implementation.
		 */
		RAWINPUT Update( const HRAWINPUT /*hrawinput*/)
		{
			RAWINPUT rData = {0};

// 			UINT pcbSize;
// 
// 			// If pData is NULL, the minimum required buffer, in bytes, is returned in pcbSize.
// 			UINT rBuff = ::GetRawInputBuffer(nullptr, &pcbSize, sizeof(RAWINPUTHEADER));
// 
// 			if (rBuff == -1) throw RawInputException("GetRawInputBuffer()");
// 
// 			std::vector<RAWINPUT> pData(pcbSize / sizeof(RAWINPUT));
// 
// 			// If rData is not NULL and the function is successful,
// 			// the return value is the number of RAWINPUT structures written to pData.
// 			rBuff = ::GetRawInputBuffer(
// 				&pData[0],
// 				&pcbSize,
// 				sizeof(RAWINPUTHEADER));
// 
// 			if (rBuff == -1) throw RawInputException("GetRawInputBuffer()");
//
//			//if (rBuff == 0) return;
//
// 			for (std::size_t i = 0; i < rBuff; ++i) {
// 				switch (pData[i].header.dwType) {
// 					case RIM_TYPEMOUSE:
// 						ri_mouse_ << pData[i];
// 						break;
// 					case RIM_TYPEKEYBOARD:
// 						ri_keyboard_ << pData[i];
// 						break;
// 					case RIM_TYPEHID:
// 						ri_hid_ << pData[i];
// 						break;
// 				}
// 			}

			return rData;
		}
	};

	template
	<
		class BufferingPolicy = Unbuffered,
#ifdef UNICODE
		class String = std::wstring
#else
		class String = std::string
#endif
	>
	class Input : public BufferingPolicy
	{
		typedef std::map<RawDevice::Handle, RawDevice::Ptr> RawDevMap;

		typedef std::vector<RAWINPUTDEVICE> DeviceVec;

	public:
		/** \brief Registers Mouse, Keyboard and Joystick/Gamepad devices.
		 *
		 * \param hwnd the window handle to register for RawInput events.
		 * \param mouse_flags Raw Input flags for mouse registering.
		 * \param keyb_flags Raw Input flags for keyboard registering.
		 * \param hid_flags Raw Input flags for hid registering.
		 */
		explicit Input( const HWND hwnd,
						const DWORD mouse_flags = 0,
						const DWORD keyb_flags = RIDEV_NOLEGACY | RIDEV_INPUTSINK,	//0,
						const DWORD hid_flags = 0 )
			: BufferingPolicy(),
			m_ri_registered_devices(),
			m_ri_devs(),
			m_onMouse(),
			m_onKeyboard()
		{
			const USHORT usage_table[] = {
				RawDevice::HID_DEVICE_SYSTEM_MOUSE,
				RawDevice::HID_DEVICE_SYSTEM_KEYBOARD,
				RawDevice::HID_DEVICE_SYSTEM_GAMEPAD,
			};

			const DWORD flags_table[] = {
				mouse_flags,
				keyb_flags,
				hid_flags,
			};

			const auto && ri_sysDevices = Input::EnumSystemDevices();

#ifndef NDEBUG
			std::basic_ofstream<String::value_type> log("RawInput.log");
			log << TEXT("Found ") << ri_sysDevices.size() << TEXT(" devices to be registered:\n");
#endif

			std::transform(
				ri_sysDevices.begin(),
				ri_sysDevices.end(),
				std::back_inserter(m_ri_registered_devices),
				[&](const RAWINPUTDEVICELIST & ridl)
				{
#ifndef NDEBUG
					log << TEXT("[") << ridl.hDevice << TEXT("] ") << this->GetDeviceName(ridl.hDevice) << TEXT("\n");
#endif

					const RID_DEVICE_INFO && info = this->GetDeviceInfo(ridl.hDevice);

					RAWINPUTDEVICE ri_dev = {
						RawDevice::HID_USAGE_PAGE,
						usage_table[info.dwType],
						flags_table[info.dwType],
						hwnd
					};

					return ri_dev;
				});

			if (::RegisterRawInputDevices(
				&m_ri_registered_devices[0],
				static_cast<UINT>(m_ri_registered_devices.size()),
				sizeof(RAWINPUTDEVICE)) == FALSE) throw RawInputException(__FUNCTION__" [Couldn't register raw input devices]");
		}

		/** \brief Unregisters all devices before proceeding.
		 */
		~Input( void )
		{
			std::for_each(
				m_ri_registered_devices.begin(),
				m_ri_registered_devices.end(),
				[&](RAWINPUTDEVICE & dev) {
					dev.dwFlags = RIDEV_REMOVE;

					dev.hwndTarget = nullptr;
				});

			::RegisterRawInputDevices(
				&m_ri_registered_devices[0],
				static_cast<UINT>(m_ri_registered_devices.size()),
				sizeof(RAWINPUTDEVICE));
		}


		/** \brief Call on WM_INPUT. Updates registered devices and fires data received events.
		 *
		 * \param hwnd the handle to the user WindowProc.
		 * \param message the message name of the user WindowProc.
		 * \param wParam the wParam of the user WindowProc.
		 * \param lParam the lParam of the user WindowProc.
		 *
		 * \return 0 if successful.
		 */
		LRESULT Update( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
		try {
			RAWINPUT rData = this->BufferingPolicy::Update(reinterpret_cast<HRAWINPUT>(lParam));

			auto it = m_ri_devs.find(rData.header.hDevice);

			if (it == m_ri_devs.end()) { // if device not found create and add new device, then read data.
				auto newDev = this->CreateDevice(rData.header);

				auto added = m_ri_devs.insert(std::make_pair(rData.header.hDevice, newDev));

				if (added.second) it = added.first;
			}

			auto device = it->second;

			device->Read(rData);

			switch (rData.header.dwType) {
			case RIM_TYPEMOUSE:
				{
				const auto & mouse = static_cast<RawMouse &>(*device.get());

				for (auto && signal : m_onMouse) {
					signal(mouse);
				}

				break;
				}

			case RIM_TYPEKEYBOARD:
				{
				const auto & keyboard = static_cast<RawKeyboard &>(*device.get());

				for (auto && signal : m_onKeyboard) {
					signal(keyboard);
				}

				break;
				}

			case RIM_TYPEHID:
			default:
				{
				const auto & hid = static_cast<RawHID &>(*device.get());

				for (auto && signal : m_onHid) {
					signal(hid);
				}
				break;
				}
			}

			return 0;
		}
		catch (...) {
			return ::DefWindowProc(hwnd, message, wParam, lParam);
		}

		/** \brief Call on WM_INPUT_DEVICE_CHANGE. Handles device arrival/removal.
		 *
		 * \param wParam the wParam of the user WindowProc.
		 * \param lParam the lParam of the user WindowProc.
		 *
		 * \return 0 if successful.
		 */
		LRESULT Change( WPARAM wParam, LPARAM lParam )
		{
			switch (GET_RAWINPUT_CODE_WPARAM(wParam)) {
			case GIDC_ARRIVAL:
				// Handle arrival

				#ifndef NDEBUG
				std::wcout << TEXT("RawDevice arrival detected: ") << this->GetDeviceName(reinterpret_cast<HANDLE>(lParam)) << std::endl;
				#endif

				break;

			case GIDC_REMOVAL:
				// Handle removal

				#ifndef NDEBUG
				std::wcout << TEXT("RawDevice removal detected: ") << this->GetDeviceName(reinterpret_cast<HANDLE>(lParam)) << std::endl;
				#endif

				break;
			}

			return 0;
		}

	public:
		/** \brief Used to connect to new RawMouse data received event.
		 * \param callable a functor to be called on data received event.
		 *
		 * \return return the Input object to allow chains of connect calls.
		 */
		Input & connect( RawMouse::Event && callable )
		{
			m_onMouse.push_back(callable);

			return *this;
		}
		
		/** \brief Used to connect to new RawKeyboard data received event.
		 * \param callable a functor to be called on data received event.
		 *
		 * \return return the Input object to allow chains of connect calls.
		 */
		Input & connect( RawKeyboard::Event && callable )
		{
			m_onKeyboard.push_back(callable);

			return *this;
		}

		/** \brief Used to connect to new RawHID data received event.
		 * \param callable a functor to be called on data received event.
		 *
		 * \return return the Input object to allow chains of connect calls.
		 */
		Input & connect( RawHID::Event && callable )
		{
			m_onHid.push_back(callable);

			return *this;
		}

	private:
		/** \brief returns a vector with all the devices connected to the system
		 *
		 * \returns a vector<RAWINPUTDEVICELIST> with all the devices connected to the system at the time of call.
		 */
		static std::vector<RAWINPUTDEVICELIST> EnumSystemDevices( void )
		{
			UINT ri_system_count;

			// Returns the number of RAWINPUTDEVICELIST structures that can be
			// contained in the buffer to which pRawInputDeviceList points
			// If this value is less than the number of devices attached to the
			// system, the function returns the actual number of devices in this
			// variable and fails with ERROR_INSUFFICIENT_BUFFER.
			if (::GetRawInputDeviceList(
				nullptr,
				&ri_system_count,
				sizeof(RAWINPUTDEVICELIST)) < 0) throw RawInputException(__FUNCTION__" [Couldn't get buffer size for system devices]");

			std::vector<RAWINPUTDEVICELIST> ri_sys_dev(ri_system_count);

			// Populates ri_sys_dev with the number of devices attached to the system.
			if (::GetRawInputDeviceList(
				&ri_sys_dev[0],
				&ri_system_count,
				sizeof(RAWINPUTDEVICELIST)) < 0) throw RawInputException(__FUNCTION__" [Couldn't get system devices]");

			return std::move(ri_sys_dev);
		}

		/** \brief queries system for device info.
		 *
		 * \param hDevice handle to a device.
		 *
		 * \return RID_DEVICE_INFO with data for the requested device.
		 */
		RID_DEVICE_INFO GetDeviceInfo( RawDevice::Handle hDevice ) const
		{
			RID_DEVICE_INFO pData;

			// If uiCommand is RIDI_DEVICEINFO, set RID_DEVICE_INFO.cbSize to
			// sizeof(RID_DEVICE_INFO) before calling GetRawInputDeviceInfo.
			pData.cbSize = sizeof(RID_DEVICE_INFO);

			UINT pcbSize = sizeof(RID_DEVICE_INFO);

			// Returns bytes copied to pData, gets input data in pData.
			if (::GetRawInputDeviceInfo(
				hDevice,
				RIDI_DEVICEINFO,
				&pData,
				&pcbSize) < 0) throw RawInputException(__FUNCTION__" [Couldn't get device info]");

			return pData;
		}

		/** \brief returns the system's device name.
		 *
		 * \param hDevice handle to the device.
		 *
		 * \return the system's device name.
		 */
		String GetDeviceName( HANDLE hDevice ) const
		{
			UINT pcbSize;

			// If pData is NULL, the function returns a value of zero.
			if (::GetRawInputDeviceInfo(
				hDevice,
				RIDI_DEVICENAME,
				nullptr,
				&pcbSize) != 0) throw RawInputException(__FUNCTION__" [Couldn't get buffer size for device name]");

			String name(pcbSize, TEXT(' '));

			// If successful, this function returns a non-negative number indicating the number of character copied to pData.
			// If pData is not large enough for the data, the function returns -1. 
			if (::GetRawInputDeviceInfo(
				hDevice,
				RIDI_DEVICENAME,
				&name[0],
				&pcbSize) < 0) throw RawInputException(__FUNCTION__" [Couldn't get device name]");

			return String(name.cbegin(), name.cend() - 1); // get rid of the terminating character.
		}

		/** \brief creates a new device.
		 *
		 * \param info contains info to create a new device.
		 *
		 * \return a RawDevice::Ptr to the new device.
		 */
		RawDevice::Ptr CreateDevice( const RAWINPUTHEADER & info )
		{
			switch (info.dwType) {
			case RIM_TYPEMOUSE:
				return std::make_shared<RawMouse>(info.hDevice);

			case RIM_TYPEKEYBOARD:
				return std::make_shared<RawKeyboard>(info.hDevice);

			case RIM_TYPEHID:
				return std::make_shared<RawHID>(info.hDevice);

			default:
				throw RawInputException(__FUNCTION__" [Received bad device type]");
			}
		}

		Input(const Input &);

		Input & operator=(const Input &);

	private:
		DeviceVec	m_ri_registered_devices; ///< Holds all the devices registered for raw input.

		RawDevMap	m_ri_devs; ///< a collection of device pointers.

		std::vector<RawMouse::Event> m_onMouse; ///< holds all the events that will be fired on new mouse data events.

		std::vector<RawKeyboard::Event> m_onKeyboard; ///< holds all the events that will be fired on new keyboard data events.

		std::vector<RawHID::Event> m_onHid; ///< holds all the events that will be fired on new hid data events.
	};
}

#endif // RawInput_h__
