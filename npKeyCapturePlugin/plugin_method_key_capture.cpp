#include "plugin_method_key_capture.h"

#include <cstdint>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>

// fileExists( filename, callback(status) )
PluginMethodKeyCapture::PluginMethodKeyCapture(NPObject* object, NPP npp) :
PluginMethod(object, npp) {
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
		if (argCount != 2 ||
			!NPVARIANT_IS_STRING(args[0]) ||
			!NPVARIANT_IS_OBJECT(args[1])) {
			NPN_SetException(
				__super::object_,
				"invalid params passed to function");
			delete clone;
			return nullptr;
		}
		clone->input_.append(
			NPVARIANT_TO_STRING(args[0]).UTF8Characters,
			NPVARIANT_TO_STRING(args[0]).UTF8Length);

		clone->callback_ = NPVARIANT_TO_OBJECT(args[1]);
		
		// add ref count to callback object so it won't delete
		NPN_RetainObject(clone->callback_);

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
	
	HKL layout = GetKeyboardLayout(0);

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
		if (isRun)
			Sleep(15);
	}	
}

// virtual
void PluginMethodKeyCapture::TriggerCallback() {
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