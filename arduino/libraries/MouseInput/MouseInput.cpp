//==============================================================================
// MouseInput.cpp - Library to interface with (PS2) mice.
// Written by Sugapes, August 2014.
// MIT license, check LICENSE file at the top-level directory for more information.
//==============================================================================

#include "MouseInput.h"

// Initialize the MouseInput object (i.e. its variables)
MouseInput::MouseInput()
{
   _mstat = 0;
   _mx = 0;
   _my = 0;
   _dx = 0;
   _dy = 0;
   _x = 0;
   _y = 0;
   _overf = 0;
   _state = NO_INIT_MOUSE;
   _polling_interval = 1000;
}

// Initialize operation of the mouse interface, namely the pins used.
void MouseInput::init(int clk, int data)
{
   _ps2_if.init(clk, data);
   _state = START_INIT;
}
// Return the X coordinate
int MouseInput::getX(void)
{
   return _x;
}

// Return the Y coordinate
int MouseInput::getY(void)
{
   return _y;
}

// Update the state machine (need to call it frequently in the main loop)
void MouseInput::update(void)
{
   // Update PS2 state machine
   _ps2_if.update();

   // State Machine - by default stay on the current state
   switch (_state) {
      // === NOT INITIALIZED ===
      case NO_INIT_MOUSE:
         // Do nothing - e.g. pins not defined
         break;
      // === INIT PS2 MOUSE SEQUENCE ===
      case START_INIT:
         _last_time = micros();
         _state = WAIT_START_INIT;
         break;
      case WAIT_START_INIT:
         if (_ps2_if.elapsedMicros(100, _last_time)) {
            _state = RESET;
         }
         break;
      case RESET:
         _ps2_if.write(0xff); // reset
         _state = WAIT_BUSY1;
         break;
      case WAIT_BUSY1:
         if (! _ps2_if.busy()) {
            _state = READ_ACK_BYTE;
         }
         break;
      case READ_ACK_BYTE:
         _ps2_if.read(); // ack byte
         _state = WAIT_BUSY2;
         break;
      case WAIT_BUSY2:
         if (! _ps2_if.busy()) {
            _state = READ_BLANK1;
         }
         break;
      case READ_BLANK1:
         _ps2_if.read(); // blank 1
         _state = WAIT_BUSY3;
         break;
      case WAIT_BUSY3:
         if (! _ps2_if.busy()) {
            _state = READ_BLANK2;
         }
         break;
      case READ_BLANK2:
         _ps2_if.read(); // blank 2
         _state = WAIT_BUSY4;
         break;
      case WAIT_BUSY4:
         if (! _ps2_if.busy()) {
            _state = REMOTE_MODE;
         }
         break;
      case REMOTE_MODE:
         _ps2_if.write(0xf0);  // remote mode
         _state = WAIT_BUSY5;
         break;
      case WAIT_BUSY5:
         if (! _ps2_if.busy()) {
            _state = ACK_REMOTE_MODE;
         }
         break;
      case ACK_REMOTE_MODE:
         _ps2_if.read();       // ack
         _state = WAIT_BUSY6;
         break;
      case WAIT_BUSY6:
         if (! _ps2_if.busy()) {
            _state = SET_SAMPLE_RATE;
         }
         break;
      case SET_SAMPLE_RATE:
         _ps2_if.write(0xf3);  // set sample rate
         _state = WAIT_BUSY7;
         break;
      case WAIT_BUSY7:
         if (! _ps2_if.busy()) {
            _state = ACK_SR;
         }
         break;
      case ACK_SR:
         _ps2_if.read();       // ack
         _state = WAIT_BUSY8;
         break;
      case WAIT_BUSY8:
         if (! _ps2_if.busy()) {
            _state = VALUE_SR;
         }
         break;
      case VALUE_SR:
         _ps2_if.write(200);   // value
         _state = WAIT_BUSY9;
         break;
      case WAIT_BUSY9:
         if (! _ps2_if.busy()) {
            _state = ACK_VAL_SR;
         }
         break;
      case ACK_VAL_SR:
         _ps2_if.read();       // ack
         _state = WAIT_BUSY10;
         break;
      case WAIT_BUSY10:
         if (! _ps2_if.busy()) {
            _state = SET_RESOLUTION;
         }
         break;
      case SET_RESOLUTION:
         _ps2_if.write(0xe8);  // set resolution
         _state = WAIT_BUSY11;
         break;
      case WAIT_BUSY11:
         if (! _ps2_if.busy()) {
            _state = ACK_RES;
         }
         break;
      case ACK_RES:
         _ps2_if.read();       // ack
         _state = WAIT_BUSY12;
         break;
      case WAIT_BUSY12:
         if (! _ps2_if.busy()) {
            _state = VALUE_RES;
         }
         break;
      case VALUE_RES:
         _ps2_if.write(3);     // value
         _state = WAIT_BUSY13;
         break;
      case WAIT_BUSY13:
         if (! _ps2_if.busy()) {
            _state = ACK_VALUE_RES;
         }
         break;
      case ACK_VALUE_RES:
         _ps2_if.read();       // ack
         _state = WAIT_BUSY14;
         break;
      case WAIT_BUSY14:
         if (! _ps2_if.busy()) {
            _last_time = micros();
            _state = WAIT_END_INIT;
         }
         break;
      case WAIT_END_INIT:
         if (_ps2_if.elapsedMicros(100, _last_time)) {
            _state = READ_DATA;
         }
         break;
      // === POLLING MOUSE FOR DATA ===
      case READ_DATA:
         _ps2_if.write(0xeb);    // read data
         _state = WAIT_BUSY15;
         break;
      case WAIT_BUSY15:
         if (! _ps2_if.busy()) {
            _state = ACK_READ;
         }
         break;
      case ACK_READ:
         _ps2_if.read();       // ack (ignore)
         _state = WAIT_BUSY16;
         break;
      case WAIT_BUSY16:
         if (! _ps2_if.busy()) {
            _state = VALUE_MSTAT;
         }
         break;
      case VALUE_MSTAT:
         _ps2_if.read();       // mstat
         _state = WAIT_BUSY17;
         break;
      case WAIT_BUSY17:
         if (! _ps2_if.busy()) {
            _mstat = _ps2_if.getByte();
            _state = VALUE_MX;
         }
         break;
      case VALUE_MX:
         _ps2_if.read();       // mstat
         _state = WAIT_BUSY18;
         break;
      case WAIT_BUSY18:
         if (! _ps2_if.busy()) {
            _mx = _ps2_if.getByte();
            _state = VALUE_MY;
         }
         break;
      case VALUE_MY:
         _ps2_if.read();       // mstat
         _state = WAIT_BUSY19;
         break;
      case WAIT_BUSY19:
         if (! _ps2_if.busy()) {
            _my = _ps2_if.getByte();
            _state = UPDATE_VALUES;
         }
         break;
      case UPDATE_VALUES:
         if (_mstat & 0x10) {
            _dx = _mx - 256;     // negative value
         } else {
            _dx = _mx;
         }
         _x += _dx;
         if (_mstat & 0x20) {
            _dy = _my - 256;     // negative value
         } else {
            _dy = _my;
         }
         _y += _dy;
         if (_mstat & 0xC0) {
            _overf = 1;
         } else {
            _overf = 0;
         }
         _last_time = micros();
         _state = WAIT_POLLING;
         break;
      case WAIT_POLLING:
         if (_ps2_if.elapsedMicros(_polling_interval, _last_time)) {
            _state = READ_DATA;
         }
         break;
   } // end switch
}
