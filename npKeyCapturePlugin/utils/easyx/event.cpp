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

/*
* Constructor
*
* */
Event::Event()
    : gamepad(-1),
        type(0),
        which(0),
        angle(0) {
    /* Intentionally left blank. */
}

/*
* Constructor
*
	* @param  The ID of the gamepad. Will be an integer between 0-3.
	* @param  The event type, e.g. EZX_PRESS or EZX_CONNECT.
	* @param  The ID of the thing that triggered this event, e.g. EZX_A or EZX_LTRIGGER.
*
* This constructor is used for non-analog events.
* Those events are the following:
* EZX_PRESS, EZX_RELEASE, EZX_CONNECT, EZX_DISCONNECT
* */
Event::Event(
    GamepadID gamepad,
    EventType type,
    ButtonID which)
    : gamepad(gamepad),
        type(type),
        which(which),
        angle(0) {
    /* Intentionally left blank. */
}

/*
* Constructor
*
	* @param  The ID of the gamepad. Will be an integer between 0-3.
	* @param  The event type, e.g. EZX_ANALOG.
	* @param  The ID of the thing that triggered this event, e.g. EZX_A or EZX_LTRIGGER.
	* @param  The angle of the analog that triggered this event.
*
* This constructor is used for analog events.
* Those events are the following:
* EZX_ANALOG
* */
Event::Event(
    GamepadID gamepad,
    EventType type,
    ButtonID which,
    int16_t angle)
    : gamepad(gamepad),
        type(type),
        which(which),
        angle(angle) {
    /* Intentionally left blank. */
}

}
