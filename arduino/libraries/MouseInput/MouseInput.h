//==============================================================================
// MouseInput.h - Library to interface with (PS2) mice.
// Written by Sugapes, August 2014.
// MIT license, check LICENSE file at the top-level directory for more information.
//==============================================================================

#ifndef MouseInput_h
#define MouseInput_h

#include "Arduino.h"
#include "PS2.h"

enum mouse_state_t {
   NO_INIT_MOUSE,
   START_INIT,
   WAIT_START_INIT,
   RESET,
   WAIT_BUSY1,
   READ_ACK_BYTE,
   WAIT_BUSY2,
   READ_BLANK1,
   WAIT_BUSY3,
   READ_BLANK2,
   WAIT_BUSY4,
   REMOTE_MODE,
   WAIT_BUSY5,
   ACK_REMOTE_MODE,
   WAIT_BUSY6,
   SET_SAMPLE_RATE,
   WAIT_BUSY7,
   ACK_SR,
   WAIT_BUSY8,
   VALUE_SR,
   WAIT_BUSY9,
   ACK_VAL_SR,
   WAIT_BUSY10,
   SET_RESOLUTION,
   WAIT_BUSY11,
   ACK_RES,
   WAIT_BUSY12,
   VALUE_RES,
   WAIT_BUSY13,
   ACK_VALUE_RES,
   WAIT_BUSY14,
   WAIT_END_INIT,
   READ_DATA,
   WAIT_BUSY15,
   ACK_READ,
   WAIT_BUSY16,
   VALUE_MSTAT,
   WAIT_BUSY17,
   VALUE_MX,
   WAIT_BUSY18,
   VALUE_MY,
   WAIT_BUSY19,
   UPDATE_VALUES,
   WAIT_POLLING
}; /* mouse_state_t */

class MouseInput
{
   public:
      MouseInput();
      void init(int clk, int data);
      int getX(void);
      int getY(void);
      void update(void);
   private:
      PS2 _ps2_if;
      int _mstat;
      int _mx;
      int _my;
      int _dx;
      int _dy;
      int _x;
      int _y;
      int _overf;
      mouse_state_t _state;
      unsigned long _last_time;
      unsigned long _polling_interval;
};

#endif /* MouseInput_h */
