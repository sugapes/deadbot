//==============================================================================
// PS2.h - Library to interface with (PS2) mice.
// Written by Sugapes, August 2014.
// MIT license, check LICENSE file at the top-level directory for more information.
//==============================================================================

#ifndef PS2_h
#define PS2_h

#include "Arduino.h"

enum ps2_state_t {
   NO_INIT_PS2,
   IDLE_STATE,
   START_WRITE,
   WAIT1,
   BRING_CLOCK_LOW,
   WAIT2,
   BRING_DATA_LOW,
   WAIT3,
   START_BIT,
   TX_WAIT_CLK_FALL,
   TX_BIT,
   TX_WAIT_CLK_HIGH,
   TX_WAIT_CLK_LOW,
   PARITY_BIT,
   PAR_WAIT_CLK_HIGH,
   PAR_WAIT_CLK_LOW,
   STOP_BIT,
   WAIT4,
   STOP_WAIT_CLK_LOW,
   WAIT_MODE_SWITCH,
   START_READ,
   WAIT5,
   RX_WAIT_CLK_FALL,
   WAIT6,
   RX_START_BIT,
   RX_WAIT_CLK_LOW,
   RX_BIT,
   RX_WAIT_CLK_HIGH,
   RX_PAR_WAIT_CLK_LOW,
   RX_PAR_WAIT_CLK_HIGH,
   RX_STOP_WAIT_CLK_LOW,
   RX_STOP_WAIT_CLK_HIGH
}; /* ps2_state_t */

class PS2
{
   public:
      PS2();
      void init(int clk, int data);
      void write(unsigned char data);
      void read(void);
      boolean busy(void);
      unsigned char getByte(void);
      void update(void);
      boolean elapsedMicros(unsigned long interval, unsigned long last_time);
   private:
      int _pin_clk;
      int _pin_data;
      int _busy;
      int _cmd_write;
      int _cmd_read;
      unsigned char _tx_data;
      unsigned char _rx_data;
      unsigned long _last_time;
      unsigned char _bit_cnt;
      unsigned char _parity;
      ps2_state_t _state;
      void setHigh(int pin);
      void setLow(int pin);
};

#endif /* PS2_h */
