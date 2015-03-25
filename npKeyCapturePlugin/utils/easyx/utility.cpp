/*
* EasyXInput
* http://tylerobrien.com
* https://github.com/TylerOBrien/EasyXInput
*
* Copyright (c) 2014 Tyler O'Brien
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* */

#include "easyx.hpp"

namespace easyx {
namespace data {

const char * const button_names[] = {
	"A", "B", "X", "Y",
	"DPAD Left", "DPAD Right", "DPAD Up", "DPAD Down",
	"Left Bumper", "Right Bumper",
	"Left Thumb", "Right Thumb",
	"Left Thumb X", "Left Thumb Y", "Right Thumb X", "Right Thumb Y",
	"Start", "Back"
};

}

/*
* data::id() returns ButtonID
*
	* @param  The name of the button (e.g. "A", "Start", "Left Bumper").
*
* Converts the passed name of the button to its numerical identifier.
* Given that this function could require several strcmp() calls it is
* only intended to be used for debugging and testing purposes.
* */
ButtonID id(const char *name) {
if (strcmp(name, data::button_names[0]) == 0) {
	return XINPUT_GAMEPAD_A;
} else if (strcmp(name, data::button_names[1]) == 0) {
	return XINPUT_GAMEPAD_B;
} else if (strcmp(name, data::button_names[2]) == 0) {
	return XINPUT_GAMEPAD_X;
} else if (strcmp(name, data::button_names[3]) == 0) {
	return XINPUT_GAMEPAD_Y;
} else if (strcmp(name, data::button_names[4]) == 0) {
	return XINPUT_GAMEPAD_DPAD_LEFT;
} else if (strcmp(name, data::button_names[5]) == 0) {
	return XINPUT_GAMEPAD_DPAD_RIGHT;
} else if (strcmp(name, data::button_names[6]) == 0) {
	return XINPUT_GAMEPAD_DPAD_UP;
} else if (strcmp(name, data::button_names[7]) == 0) {
	return XINPUT_GAMEPAD_DPAD_DOWN;
} else if (strcmp(name, data::button_names[8]) == 0) {
	return XINPUT_GAMEPAD_LEFT_SHOULDER;
} else if (strcmp(name, data::button_names[9]) == 0) {
	return XINPUT_GAMEPAD_RIGHT_SHOULDER;
} else if (strcmp(name, data::button_names[10]) == 0) {
	return XINPUT_GAMEPAD_LEFT_THUMB;
} else if (strcmp(name, data::button_names[11]) == 0) {
	return XINPUT_GAMEPAD_RIGHT_THUMB;
} else if (strcmp(name, data::button_names[12]) == 0) {
	return EASYX_LTHUMB_X;
} else if (strcmp(name, data::button_names[13]) == 0) {
	return EASYX_LTHUMB_Y;
} else if (strcmp(name, data::button_names[14]) == 0) {
	return EASYX_RTHUMB_X;
} else if (strcmp(name, data::button_names[15]) == 0) {
	return EASYX_RTHUMB_Y;
} else if (strcmp(name, data::button_names[16]) == 0) {
	return XINPUT_GAMEPAD_START;
} else if (strcmp(name, data::button_names[17]) == 0) {
	return XINPUT_GAMEPAD_BACK;
}

return 0;
}

/*
* data::name() returns std::string
*
	* @param  The id of the button.
*
* Converts the passed numerical identifier of the button to its name,
* which is a string. Examples of names being "A", "Start", or "Left Trigger".
* */
std::string name(const ButtonID id) {
	switch (id) {
		case XINPUT_GAMEPAD_A: return data::button_names[0];
		case XINPUT_GAMEPAD_B: return data::button_names[1];
		case XINPUT_GAMEPAD_X: return data::button_names[2];
		case XINPUT_GAMEPAD_Y: return data::button_names[3];
		case XINPUT_GAMEPAD_DPAD_LEFT: return data::button_names[4];
		case XINPUT_GAMEPAD_DPAD_RIGHT: return data::button_names[5];
		case XINPUT_GAMEPAD_DPAD_UP: return data::button_names[6];
		case XINPUT_GAMEPAD_DPAD_DOWN: return data::button_names[7];
		case XINPUT_GAMEPAD_LEFT_SHOULDER: return data::button_names[8];
		case XINPUT_GAMEPAD_RIGHT_SHOULDER: return data::button_names[9];
		case XINPUT_GAMEPAD_LEFT_THUMB: return data::button_names[10];
		case XINPUT_GAMEPAD_RIGHT_THUMB: return data::button_names[11];
		case EASYX_LTHUMB_X: return data::button_names[12];
		case EASYX_LTHUMB_Y: return data::button_names[13];
		case EASYX_RTHUMB_X: return data::button_names[14];
		case EASYX_RTHUMB_Y: return data::button_names[15];
		case XINPUT_GAMEPAD_START: return data::button_names[16];
		case XINPUT_GAMEPAD_BACK: return data::button_names[17];
		default: return std::string();
	}
}

}
