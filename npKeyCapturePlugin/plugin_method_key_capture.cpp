#include "plugin_method_key_capture.h"

#include <cstdint>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>

#include <RawInput.h>
#include <iomanip>
#include <memory>

#include <tchar.h>
#include "utils\Encoders.h"

int32_t id = 0;	// there is quite likely a better way to do this...
std::map<int32_t, PluginMethodKeyCapture*> instanceMap;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND SetupWindow(LPCSTR);
HWND hwnd = NULL;
typedef RawInput::Input<> InputSys; // Instantiation with default template parameter.
std::shared_ptr<InputSys> input = NULL;

PluginMethodKeyCapture::PluginMethodKeyCapture(NPObject* object, NPP npp) :
PluginMethod(object, npp) {
}

bool PluginMethodKeyCapture::DeleteInstance(int32_t id_in)	// static
{
	if (instanceMap.find(id_in) == instanceMap.end())
		return false;

	PluginMethodKeyCapture* instance = instanceMap[id_in];
	instance->callback_ = nullptr;

	input.reset(); // Its better to destroy the RawInput object before unregistering the class.
	
	/*BOOL ret;
	if (hwnd != NULL)
	{
		ret = DestroyWindow(hwnd);
		if (ret == 0)
			NPN_SetException(instance->__super::object_, "Window destroying failed!");

		hwnd = NULL;
	}

	ret = ::UnregisterClass("OW_MsgWnd_Thorwe", ::GetModuleHandle(nullptr));
	if (ret == 0)
		NPN_SetException(instance->__super::object_, "Window class unregistration failed"); */

	hwnd = NULL;
	input = NULL;

	// delete instance;
	instanceMap.erase(id_in);
	return true;
}

//virtual 
PluginMethod* PluginMethodKeyCapture::Clone(
	NPObject* object,
	NPP npp,
	const NPVariant *args,
	uint32_t argCount,
	NPVariant *result) {

	PluginMethodKeyCapture* clone =
		new PluginMethodKeyCapture(object, npp);

	try {
		if (argCount != 3 ||
			!NPVARIANT_IS_STRING(args[0]) ||
			!NPVARIANT_IS_OBJECT(args[1]) ||
			!NPVARIANT_IS_OBJECT(args[2])) {
			NPN_SetException(
				__super::object_,
				"invalid params passed to function");
			delete clone;
			return nullptr;
		}
		clone->input_.append(
			NPVARIANT_TO_STRING(args[0]).UTF8Characters,
			NPVARIANT_TO_STRING(args[0]).UTF8Length);

		clone->callback_ = NPVARIANT_TO_OBJECT(args[2]);

		/* HKL layout = GetKeyboardLayout(0);

		std::map< SHORT, char > keys;
		std::map< SHORT, char >::const_iterator keyit;

		std::string buf; // Have a buffer string
		std::stringstream ss(clone->input_); // Insert the string into a stream
		std::vector<std::string> tokens; // Create vector to hold our words
		while (ss >> buf)
			tokens.push_back(buf);	// space delimiter!

		SHORT keyCode;
		std::vector<std::string>::const_iterator inputit;
		for (inputit = tokens.begin(); inputit != tokens.end(); ++inputit)  {
			char c = inputit->at(0);	// take first char of string, not entirely fool proof...
			keyCode = VkKeyScanEx(c, layout);
			keys.insert(std::pair<SHORT, char>(keyCode, c));
		}*/

		// add ref count to callback object so it won't delete
		NPN_RetainObject(clone->callback_);


		// id + callback
		clone->id_ = id;
		++id;

		instanceMap.insert(std::make_pair(clone->id_, clone));
		NPVariant arg;
		NPVariant ret_val;
		NPObject* id_callback = NPVARIANT_TO_OBJECT(args[1]);

		INT32_TO_NPVARIANT(
			clone->id_,
			arg
			);

		// fire callback
		NPN_InvokeDefault(
			__super::npp_,
			id_callback,
			&arg,
			1,
			&ret_val);

		NPN_ReleaseVariantValue(&ret_val);
		// / id + callback

		return clone;
	}
	catch (...) {

	}

	delete clone;
	return nullptr;
}

// virtual
bool PluginMethodKeyCapture::HasCallback() {
	return (nullptr != callback_);
}

// virtual
void PluginMethodKeyCapture::Execute() {
	if (hwnd == NULL) {
		if ((hwnd = SetupWindow("OW_MsgWnd_Thorwe_" + id_)) == NULL) {
			NPN_SetException(__super::object_, "could not create window");
			return;
		}
	}
	if (input == NULL) {
		input = std::make_shared<InputSys>(hwnd);
	

		(*input)
			/*.connect(RawInput::RawMouse::Event([&]( const RawInput::RawMouse & mouse )
			{
			const char delim = ',';

			std::wcout
			<< TEXT("[") << std::hex << mouse.GetHandle() << TEXT("]") << std::dec

			<< std::dec << std::setw(4) << mouse.GetData().usFlags << delim

			<< std::hex << std::setw(8) << mouse.GetData().ulButtons << delim

			<< std::dec << std::setw(4) << mouse.GetData().usButtonFlags << delim

			<< std::dec << std::setw(4) << static_cast<short>(mouse.GetData().usButtonData) << delim

			<< std::dec << std::setw(4) << mouse.GetData().ulRawButtons << delim

			<< std::dec << std::setw(4) << mouse.GetData().lLastX << delim
			<< std::dec << std::setw(4) << mouse.GetData().lLastY << delim

			<< std::dec << std::setw(4) << mouse.GetData().ulExtraInformation

			<< (mouse.Button(RawInput::RawMouse::BUTTON_1_DOWN|RawInput::RawMouse::BUTTON_2_DOWN) ? "!" : "")

			<< (mouse.GetWheelDelta() == 0 ? ' ' :
			mouse.GetWheelDelta() <  0 ? '<' : '>')

			<< std::endl;
			}))*/
			.connect(RawInput::RawKeyboard::Event([&](const RawInput::RawKeyboard & keyboard)
		{
			const char delim = ',';
			HKL layout = GetKeyboardLayout(0);
			USHORT vkey = keyboard.GetData().VKey;
			std::wstring chars_ws;
			if (vkey == VK_CONTROL)
				chars_ws = L"CTRL";
			else if (vkey == VK_SHIFT)
				chars_ws = L"SHIFT";
			else if (vkey == VK_MENU)
				chars_ws = L"ALT";
			else if (vkey == VK_F1)
				chars_ws = L"F1";
			else if (vkey == VK_F2)
				chars_ws = L"F2";
			else if (vkey == VK_F3)
				chars_ws = L"F3";
			else if (vkey == VK_F4)
				chars_ws = L"F4";
			else if (vkey == VK_F5)
				chars_ws = L"F5";
			else if (vkey == VK_F6)
				chars_ws = L"F6";
			else if (vkey == VK_F7)
				chars_ws = L"F7";
			else if (vkey == VK_F8)
				chars_ws = L"F8";
			else if (vkey == VK_F9)
				chars_ws = L"F9";
			else if (vkey == VK_F10)
				chars_ws = L"F10";
			else if (vkey == VK_F11)
				chars_ws = L"F11";
			else if (vkey == VK_F12)
				chars_ws = L"F12";
			else if (vkey == VK_ESCAPE)
				chars_ws = L"ESC";
			else if (vkey == VK_RETURN)
				chars_ws = L"RETURN";
			else if (vkey == VK_SPACE)
				chars_ws == L"SPACE";
			else if (vkey == VK_TAB)
				chars_ws == L"TAB";
			else if (vkey == VK_ACCEPT)
				chars_ws == L"ACCEPT";
			else if (vkey == VK_ADD)
				chars_ws == L"ADD";
			else if (vkey == VK_APPS)
				chars_ws == L"APPS";
			else {
				//UINT vkey = MapVirtualKeyEx(keyboard.GetData().VKey, MAPVK_VK_TO_CHAR, layout);
				std::vector<BYTE> keys(256, 0);
				int sc = MapVirtualKeyEx(vkey, MAPVK_VK_TO_VSC, layout);
				const int BUFFER_LENGTH = 2; //Length of the buffer
				WCHAR chars[BUFFER_LENGTH];
				ToUnicodeEx(vkey, sc, keys.data(), chars, BUFFER_LENGTH, 0, layout);
				chars[1] = L'\0';
				chars_ws = std::wstring((wchar_t*)chars);
			}

			std::stringstream s_out;
			s_out
				<< TEXT("{")
				
				// << "\"KeyboardHandle\":\"" << std::hex << keyboard.GetHandle() << "\"" << delim

				<< "\"MakeBreak\":\"" << std::dec << (keyboard.GetData().Flags & RI_KEY_BREAK ? TEXT("[B]") /*Break*/ : TEXT("[M]") /*Make*/) << "\"" << delim
				//<< L"\"MakeBreak\":\"" << std::dec << (keyboard.GetData().Flags & RI_KEY_BREAK ? L"up" /*Break*/ : L"down" /*Make*/) << "\"" << delim
				// << "\"MakeCode\":\"" << std::hex << std::setw(4) << keyboard.GetData().MakeCode << "\"" << delim
				// << "\"Flags\":" << std::dec << std::setw(4) << keyboard.GetData().Flags << delim
				// << std::dec << std::setw(4) << keyboard.GetData().Reserved << delim
				// << "\"VKey\":\"" << std::hex << std::setw(4) << keyboard.GetData().VKey << "\"" << delim
				// << "\"Message\":\"" << std::hex << std::setw(4) << keyboard.GetData().Message << "\"" << delim
				// << "\"ExtraInfo\":" << std::dec << std::setw(4) << keyboard.GetData().ExtraInformation << delim
				//<< "\"Char\":\"" << CHAR(vkey) << "\""
				<< "\"Char\":\"" << utils::Encoders::utf8_encode(chars_ws) << "\""
				//<< L"\"Char\":\"" << chars << L"\""

				<< TEXT("}");
				//<< L"}";
				
			/*auto vk = keyboard.GetData().VKey;
			if (vk < 'A' || vk > 'Z')
				s_out << '(' << std::hex << vk << ')' << std::endl;
			else
				s_out << '(' << char(vk) << ')' << std::endl;*/

			std::string out = s_out.str();
			if (out.compare(output_) != 0) {
				output_ = out;
				done_ = true;
			}
			// if (keyboard.KeyDown(VK_SPACE) || keyboard.KeyUp(VK_END)) ::PostMessage(hwnd, WM_CLOSE, 0, 0);
		}));
		/*
		.connect(RawInput::RawHID::Event([&](const RawInput::RawHID & hid)
		{
		const char delim = ',';
		std::stringstream s_out;

		s_out
		<< TEXT("[") << std::hex << hid.GetHandle() << TEXT("] ")
		<< std::dec
		<< hid.GetData().dwSizeHid << delim
		<< hid.GetData().dwCount << delim
		<< hid.GetData().bRawData // bRawData is actually an array with sizeof(dwSizeHid*dwCount)

		<< std::endl;

		output_ = s_out.str();
		}));*/

	}

	done_ = false;
	MSG msg;
	do {
		while (HasCallback() && (!::PeekMessage(&msg, hwnd, 0u, 0u, PM_NOREMOVE))) {
			::WaitMessage();
		}

		while (HasCallback() && (!done_) && (::PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))) {
			if (!::IsDialogMessage(hwnd, &msg)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);

				if (msg.message == WM_QUIT) done_ = true;
			}
		}
	} while (!done_);

	/* HKL layout = GetKeyboardLayout(0);

	std::map< SHORT, char > keys;
	std::map< SHORT, char >::const_iterator keyit;

	std::string buf; // Have a buffer string
	std::stringstream ss(input_); // Insert the string into a stream
	std::vector<std::string> tokens; // Create vector to hold our words
	while (ss >> buf)
	tokens.push_back(buf);	// space delimiter!

	SHORT keyCode;
	std::vector<std::string>::const_iterator inputit;
	for (inputit = tokens.begin(); inputit != tokens.end(); ++inputit)  {
	char c = inputit->at(0);	// take first char of string, not entirely fool proof...
	keyCode = VkKeyScanEx(c, layout);
	keys.insert(std::pair<SHORT, char>(keyCode, c));
	}

	std::int16_t key_state;
	//bool shift;
	SHORT shift = -1;
	SHORT alt = -1;
	SHORT strg = -1;

	SHORT key;
	SHORT shiftState;

	bool isRun = true;
	while (isRun)
	{
	// LSB: & 1
	// MSB: <0 the MSB of a positive integer is always 0, while the MSB of a negative integer is always 1.
	bool isShiftPressed = (GetAsyncKeyState(VK_SHIFT)) < 0;
	bool isCtrlPressed = (GetAsyncKeyState(VK_CONTROL)) < 0;
	bool isAltPressed = (GetAsyncKeyState(VK_MENU)) < 0;
	SHORT currentShiftState = 0x00 | ((isShiftPressed) ? 1 : 0) | ((isCtrlPressed) ? 2 : 0) | ((isAltPressed) ? 4 : 0);

	for (keyit = keys.begin(); keyit != keys.end(); ++keyit)
	{
	keyCode = keyit->first;
	key = keyCode & 0xFF;	// low byte
	// SHORT high = (keyCode >> 8) & 0xff;
	// input=low | (high<<8) to make them together again

	shiftState = (keyCode >> 8) & 0xff;
	key_state = GetAsyncKeyState(key);

	if ((key_state < 0) && (shiftState == currentShiftState) && (last_keyCode != keyCode))
	{
	std::stringstream out;
	out << "" << keyit->second;
	out << " currentShiftState: " << currentShiftState;
	out << " computedShiftState: " << shiftState;
	out << " isShiftPressed: " << ((isShiftPressed) ? "true" : "false") << " " << GetAsyncKeyState(VK_SHIFT);

	last_keyCode = keyCode;

	output_ = out.str();

	isRun = false;
	break;
	}
	else if ((last_keyCode == keyCode) && (key_state >=0))
	{
	last_keyCode = 0;
	// we could send a keyup event here
	}
	}

	// Sleep() requires WinXP or later (sleep 15 milliseconds)
	// Can be removed entirely to capture Yubikey, etc.
	// but may be CPU intensive
	if (isRun) {
	Sleep(15);
	isRun = HasCallback();
	}
	}*/
}

// virtual
void PluginMethodKeyCapture::TriggerCallback() {
	if (!HasCallback())
		return;

	NPVariant arg;
	NPVariant ret_val;

	STRINGN_TO_NPVARIANT(
		output_.c_str(),
		output_.size(),
		arg);

	// fire callback
	NPN_InvokeDefault(
		__super::npp_,
		callback_,
		&arg,
		1,
		&ret_val);

	NPN_ReleaseVariantValue(&ret_val);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		::DestroyWindow(hWnd); return 0;

	case WM_DESTROY:
		::PostQuitMessage(0); return 0;

	case WM_INPUT_DEVICE_CHANGE:
		return input->Change(wParam, lParam); // WIP - Only for Windows Vista or greater.

	case WM_INPUT:
		return input->Update(hWnd, message, wParam, lParam);

	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
}

HWND SetupWindow(LPCSTR name)
{
	/*HWND hWnd = FindWindow(name, name);
	if (hWnd != NULL)
		return hWnd;*/
		
	HINSTANCE hInstance = ::GetModuleHandle(nullptr);

	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = name;

	RegisterClass(&wc);
	return CreateWindow(wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
}