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

#ifndef _EASYX_EVENT_HPP_
#define _EASYX_EVENT_HPP_

#define EASYX_EVENT_PRESS      1
#define EASYX_EVENT_RELEASE    2
#define EASYX_EVENT_DOWN       3
#define EASYX_EVENT_CONNECT    4
#define EASYX_EVENT_DISCONNECT 5

namespace easyx {

/*
 * A generic catch-all object for any possible event.
 * The angle member is only used for analog events (the triggers and sticks), so it has its
 * own constructor. If not in use (i.e. a non-analog event) then angle will always equal zero.
 * */
class Event {
public:
    GamepadID gamepad;
    EventType type;
    int16_t angle;
    ButtonID which;

    Event();
    Event(GamepadID gamepad, EventType type, ButtonID which);
    Event(GamepadID gamepad, EventType type, ButtonID which, int16_t angle);
};

}

#endif