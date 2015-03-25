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

#ifndef _EASYX_HPP_
#define _EASYX_HPP_

#define EASYX_A         XINPUT_GAMEPAD_A
#define EASYX_B         XINPUT_GAMEPAD_B
#define EASYX_X         XINPUT_GAMEPAD_X
#define EASYX_Y         XINPUT_GAMEPAD_Y
#define EASYX_UP        XINPUT_GAMEPAD_DPAD_UP
#define EASYX_DOWN      XINPUT_GAMEPAD_DPAD_DOWN
#define EASYX_LEFT      XINPUT_GAMEPAD_DPAD_LEFT
#define EASYX_RIGHT     XINPUT_GAMEPAD_DPAD_RIGHT
#define EASYX_START     XINPUT_GAMEPAD_START
#define EASYX_BACK      XINPUT_GAMEPAD_BACK
#define EASYX_LBUMPER   XINPUT_GAMEPAD_LEFT_SHOULDER
#define EASYX_RBUMPER   XINPUT_GAMEPAD_RIGHT_SHOULDER
#define EASYX_LTHUMB    XINPUT_GAMEPAD_LEFT_THUMB
#define EASYX_RTHUMB    XINPUT_GAMEPAD_RIGHT_THUMB
#define EASYX_LTHUMB_X  0x10AA
#define EASYX_LTHUMB_Y  0x20AA
#define EASYX_RTHUMB_X  0x10BB
#define EASYX_RTHUMB_Y  0x20BB
#define EASYX_LTRIGGER  0x10CC
#define EASYX_RTRIGGER  0x20CC
#define EASYX_GAMEPAD_1 0
#define EASYX_GAMEPAD_2 1
#define EASYX_GAMEPAD_3 2
#define EASYX_GAMEPAD_4 3

#define EASYX_LTHUMB_DEADZONE XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
#define EASYX_RTHUMB_DEADZONE XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <string.h>
#include <windows.h>
#include <xinput.h>

namespace easyx {

class Event;

typedef uint8_t GamepadID;
typedef uint16_t ButtonID;
typedef int16_t EventType;
typedef Event* PEvent;

void update();
bool poll(PEvent evt);
	
void set_analog_threshold(GamepadID controller, ButtonID id, int16_t amount);

void set_vibration(GamepadID controller, uint16_t amount);
void set_vibration(GamepadID controller, uint16_t left, uint16_t right);
void set_vibration_level(GamepadID controller, float amount);
void set_vibration_level(GamepadID controller, float left, float right);

}

#include "event.hpp"
#include "utility.hpp"

#endif
