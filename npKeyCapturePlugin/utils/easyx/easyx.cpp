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

#include <queue>

#define LANALOG_THRESHOLD_X   (XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE + data::thresholds[id][0])
#define LANALOG_THRESHOLD_Y   (XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE + data::thresholds[id][1])
#define RANALOG_THRESHOLD_X   (XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE + data::thresholds[id][2])
#define RANALOG_THRESHOLD_Y   (XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE + data::thresholds[id][3])
#define LTRIGGER_ANGLE(ID)    data::gamepad_states[ID].Gamepad.bLeftTrigger
#define RTRIGGER_ANGLE(ID)    data::gamepad_states[ID].Gamepad.bRightTrigger

namespace easyx {
namespace data {

const ButtonID button_ids[] = {
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK,
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER
};
		
bool connections[XUSER_MAX_COUNT];
XINPUT_STATE gamepad_states[XUSER_MAX_COUNT];
bool button_states[XUSER_MAX_COUNT][14];
int16_t thresholds[XUSER_MAX_COUNT][6];
int16_t stickangles[XUSER_MAX_COUNT][4];
uint8_t triggerangles[XUSER_MAX_COUNT][2];

std::queue<Event> events;

}

namespace detect {

/*
* easyx::detect::connect() returns nothing
*
	* @param  The id of the gamepad.
*
* Determines if the passed gamepad has just connected.
* */
void connect(GamepadID id) {
	if (data::connections[id] == false) {
		data::connections[id] = true;
		data::events.push(Event(id, EASYX_EVENT_CONNECT, id));
	}
}

/*
* easyx::detect::disconnect() returns nothing
*
	* @param  The id of the gamepad.
*
* Determines if the passed gamepad has just disconnected.
* */
void disconnect(GamepadID id) {
	if (data::connections[id]) {
		data::connections[id] = false;
		data::events.push(Event(id, EASYX_EVENT_DISCONNECT, id));
	}
}

/*
* easyx::detect::triggers() returns nothing
*
	* @param  The id of the gamepad.
*
* Determines if the passed gamepad has had one of, or both of, its
* triggers pressed down at any angle other than zero. The last two
* lines are necessary for the function to determine if the current trigger
* is different from the previous trigger state.
* */
void triggers(GamepadID id) {
	if (LTRIGGER_ANGLE(id)) {
		data::events.push(Event(id, EASYX_EVENT_DOWN, EASYX_LTRIGGER, LTRIGGER_ANGLE(id)));
	} else if(data::triggerangles[id][0]) {
		data::events.push(Event(id, EASYX_EVENT_RELEASE, EASYX_LTRIGGER));
	}

	if (RTRIGGER_ANGLE(id)) {
		data::events.push(Event(id, EASYX_EVENT_DOWN, EASYX_RTRIGGER, RTRIGGER_ANGLE(id)));
	} else if(data::triggerangles[id][1]) {
		data::events.push(Event(id, EASYX_EVENT_RELEASE, EASYX_RTRIGGER));
	}
			
	data::triggerangles[id][0] = LTRIGGER_ANGLE(id);
	data::triggerangles[id][1] = RTRIGGER_ANGLE(id);
}

/*
* easyx::detect::sticks() returns nothing
*
       * @param  The id of the gamepad.
*
* Determines the state of both of the analog sticks, on both the
* X and Y axes. Because of the nature of determining if an analog
* stick is considered pressed or stationary, preprocessor definitions
* are used to cut down of the verbosity of the if statements. The
* definitions can be found at the beginning of this source file.
*
* A loop could have been used in this function, but since it is very
* unlikely that XInput will ever support more than two analog sticks
* one is not used for the sake of improving performance.
* */
void sticks(GamepadID id) {
	if (data::gamepad_states[id].Gamepad.sThumbLX >= LANALOG_THRESHOLD_X || data::gamepad_states[id].Gamepad.sThumbLX <= -LANALOG_THRESHOLD_X) {
		data::events.push(Event(id, EASYX_EVENT_DOWN, EASYX_LTHUMB_X, data::gamepad_states[id].Gamepad.sThumbLX));
		data::stickangles[id][0] = data::gamepad_states[id].Gamepad.sThumbLX;
	} else if(data::stickangles[id][0]) {
		data::events.push(Event(id, EASYX_EVENT_RELEASE, EASYX_LTHUMB_X));
		data::stickangles[id][0] = 0;
	}

	if (data::gamepad_states[id].Gamepad.sThumbLY >= LANALOG_THRESHOLD_Y || data::gamepad_states[id].Gamepad.sThumbLY <= -LANALOG_THRESHOLD_Y) {
		data::events.push(Event(id, EASYX_EVENT_DOWN, EASYX_LTHUMB_Y, data::gamepad_states[id].Gamepad.sThumbLY));
		data::stickangles[id][1] = data::gamepad_states[id].Gamepad.sThumbLY;
	} else if(data::stickangles[id][1]) {
		data::events.push(Event(id, EASYX_EVENT_RELEASE, EASYX_LTHUMB_Y));
		data::stickangles[id][1] = 0;
	}

	if (data::gamepad_states[id].Gamepad.sThumbRX >= RANALOG_THRESHOLD_X || data::gamepad_states[id].Gamepad.sThumbRX <= -RANALOG_THRESHOLD_X) {
		data::events.push(Event(id, EASYX_EVENT_DOWN, EASYX_RTHUMB_X, data::gamepad_states[id].Gamepad.sThumbRX));
		data::stickangles[id][2] = data::gamepad_states[id].Gamepad.sThumbRX;
	} else if(data::stickangles[id][2]) {
		data::events.push(Event(id, EASYX_EVENT_RELEASE, EASYX_RTHUMB_X));
		data::stickangles[id][2] = 0;
	}

	if (data::gamepad_states[id].Gamepad.sThumbRY >= RANALOG_THRESHOLD_Y || data::gamepad_states[id].Gamepad.sThumbRY <= -RANALOG_THRESHOLD_Y) {
		data::events.push(Event(id, EASYX_EVENT_DOWN, EASYX_RTHUMB_Y, data::gamepad_states[id].Gamepad.sThumbRY));
		data::stickangles[id][3] = data::gamepad_states[id].Gamepad.sThumbRY;
	} else if(data::stickangles[id][3]) {
		data::events.push(Event(id, EASYX_EVENT_RELEASE, EASYX_RTHUMB_Y));
		data::stickangles[id][3] = 0;
	}
}

/*
* easyx::buttons() returns nothing
*
       * @param  The id of the gamepad.
*
* Determines the state of any of the pressable buttons (e.g. A, B, X, Y,
* Start, Left Trigger). Only the buttons detected in this function are
* applicable to all three PRESS, DOWN, and RELEASE events.
* */
void buttons(GamepadID id) {
	for (char i=0; i<14; ++i) {
		if (data::gamepad_states[id].Gamepad.wButtons & data::button_ids[i]) {
			if (data::button_states[id][i] == false) {
				data::button_states[id][i] = true;
				data::events.push(Event(id, EASYX_EVENT_PRESS, data::button_ids[i]));
			}
			data::events.push(Event(id, EASYX_EVENT_DOWN, data::button_ids[i]));
		} else if (data::button_states[id][i]) {
			data::button_states[id][i] = false;
			data::events.push(Event(id, EASYX_EVENT_RELEASE, data::button_ids[i]));
		}
	}
}
}

/*
* easyx::update() returns nothing
*
* Updates all gamepads, detecting any connections, disconnections, or inputs.
* */
void update() {
	for (GamepadID i=0; i<XUSER_MAX_COUNT; ++i) {
		if (XInputGetState(i, &data::gamepad_states[i]) == ERROR_SUCCESS) {
			detect::connect(i);
			detect::buttons(i);
			detect::triggers(i);
			detect::sticks(i);
		} else {
			detect::disconnect(i);
		}
	}
}

/*
* easyx::poll() returns bool
*
	* @param  Pointer to the object where the event should be stored.
*
* Is used in a loop, typically a while, to pop all events out of
* the event queue. Events will popped off in the order that they
* occurred (i.e. FIFO order).
* */
bool poll(PEvent evt) {
	if (data::events.empty() == false) {
		*evt = data::events.front();
		data::events.pop();
		return true;
	}

	*evt = Event();
	return false;
}

/*
* easyx::set_analog_threshold() returns nothing
*
	* @param  The id of the gamepad to have its threshold set.
	* @param  The id of the analog stick(s) to set.
	* @param  The new threshold.
*
* Sets the threshold that the given analog stick(s) must pass for
* it/them to be considered moved. By default the threshold is the
* XINPUT_GAMEPAD_LEFT/RIGHT_THUMB_DEADZONE values. The threshold set
* by this function will be added to those values, so passing the negative
* equivalent of the XInput deadzone definitions will effectively set
* the threshold to zero.
* */
void set_analog_threshold(GamepadID gamepad, ButtonID id, int16_t amount) {
	switch (id) {
	case EASYX_LTHUMB:
		data::thresholds[gamepad][0] = amount;
		data::thresholds[gamepad][1] = amount;
		break;
	case EASYX_RTHUMB:
		data::thresholds[gamepad][2] = amount;
		data::thresholds[gamepad][3] = amount;
		break;
	case EASYX_LTHUMB_X:
		data::thresholds[gamepad][0] = amount;
		break;
	case EASYX_LTHUMB_Y:
		data::thresholds[gamepad][1] = amount;
		break;
	case EASYX_RTHUMB_X:
		data::thresholds[gamepad][2] = amount;
		break;
	case EASYX_RTHUMB_Y:
		data::thresholds[gamepad][3] = amount;
		break;
	}
}

/*
* easyx::set_vibration() returns nothing
*
	* @param  The id of the gamepad to vibrate.
	* @param  The amount to vibrate both sides.
*
* Vibrates the passed gamepad by the passed amount.
* */
void set_vibration(GamepadID gamepad, uint16_t amount) {
	set_vibration(gamepad, amount, amount);
}

/*
* easyx::set_vibration() returns nothing
*
	* @param  The id of the gamepad to vibrate.
	* @param  The amount to vibrate the left side.
	* @param  The amount to vibrate the right side.
*
* Vibrates the passed gamepad by the passed left/right amounts.
* */
void set_vibration(GamepadID gamepad, uint16_t left, uint16_t right) {
	XINPUT_VIBRATION vibrate;
	ZeroMemory(&vibrate, sizeof(XINPUT_VIBRATION));

	vibrate.wLeftMotorSpeed = left;
	vibrate.wRightMotorSpeed = right;

	XInputSetState(gamepad, &vibrate);
}

/*
* easyx::set_vibration_level() returns nothing
*
	* @param  The id of the gamepad to vibrate.
	* @param  The amount to vibrate both sides.
*
* Vibrates the passed gamepad by the passed amount. Unlike the
* setVibration() functions, this is based on the percentage of how
* much to vibrate. In other words 0.0f would mean 0% vibration,
* and 1.0f would mean 100% vibration.
* */
void set_vibration_level(GamepadID gamepad, float amount) {
	set_vibration(gamepad, uint16_t(UINT16_MAX * amount), uint16_t(UINT16_MAX * amount));
}

/*
* easyx::set_vibration_level() returns nothing
*
	* @param  The id of the gamepad to vibrate.
	* @param  The amount to vibrate the left side.
	* @param  The amount to vibrate the right side.
*
* Vibrates the passed gamepad by the passed left/right amounts.
* Unlike the setVibration() functions, this is based on the percentage of how
* much to vibrate. In other words 0.0f would mean 0% vibration,
* and 1.0f would mean 100% vibration.
* */
void set_vibration_level(GamepadID gamepad, float left, float right) {
	set_vibration(gamepad, uint16_t(UINT16_MAX * left), uint16_t(UINT16_MAX * right));
}

}
