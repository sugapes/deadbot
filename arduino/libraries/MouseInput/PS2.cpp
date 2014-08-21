//==============================================================================
// PS2.cpp - Library to read and write bytes on the PS2 interface.
// Written by Sugapes, August 2014.
// MIT license, check LICENSE file at the top-level directory for more information.
//==============================================================================

#include "PS2.h"

PS2::PS2()
{
   _pin_clk  = -1;
   _pin_data = -1;
   _busy = 0;
   _cmd_write = 0;
   _cmd_read = 0;
   _tx_data = 0;
   _rx_data = 0;
   _state = NO_INIT_PS2;
   setHigh(_pin_clk);
   setHigh(_pin_data);
}

// Initialize operation of the PS2 interface, namely the pins used.
// The clock and data pins can be wired directly to the PS2 connector.
// No external parts are needed.
void PS2::init(int clk, int data)
{
   _pin_clk  = clk;
   _pin_data = data;
   _state = IDLE_STATE;
}

// Initiate a write (byte) on the PS2 interface
void PS2::write(unsigned char data)
{
   _busy = 1;
   _tx_data = data;
   _cmd_write = 1;
}

// Initiate a read (byte) on the PS2 interface
void PS2::read(void)
{
   _busy = 1;
   _cmd_read = 1;
}

// Return the status of the current operation
boolean PS2::busy(void)
{
   return _busy;
}

// Return last byte read
unsigned char PS2::getByte(void)
{
   return _rx_data;
}

// Set pin to high by changing to input mode and enabling the internal pullup
void PS2::setHigh(int pin)
{
   pinMode(pin, INPUT);
   digitalWrite(pin, HIGH);
}

// Set pin to low by driving it (open drain)
void PS2::setLow(int pin)
{
   pinMode(pin, OUTPUT);
   digitalWrite(pin, LOW);
}

boolean PS2::elapsedMicros(unsigned long interval, unsigned long last_time)
{
   unsigned long current_time = micros();
   // WARNING: need to handle wrap around of the micros value
   if (current_time - last_time > interval) {
      return 1;
   } else {
      return 0;
   }
}

// Update state machine (need to call it frequently in the main loop)
void PS2::update(void)
{
   unsigned char bit;

   // State Machine - by default stay on the current state
   switch (_state) {
      // === NOT INITIALIZED ===
      case NO_INIT_PS2:
         // Do nothing - e.g. pins not defined
         break;
      // === IDLE ===
      case IDLE_STATE:
         if ( _cmd_write == 1 ) {
            _state = START_WRITE;
         } else if ( _cmd_read == 1 ) {
            _state = START_READ;
         };
         break;
      // === WRITE BYTE ===
      case START_WRITE:
         setHigh(_pin_data);
         setHigh(_pin_clk);
         _last_time = micros();
         _state = WAIT1;
         break;
      case WAIT1:
         if (elapsedMicros(300, _last_time)) {
            _state = BRING_CLOCK_LOW;
         }
         break;
      case BRING_CLOCK_LOW:
         setLow(_pin_clk);
         _last_time = micros();
         _state = WAIT2;
         break;
      case WAIT2:
         if (elapsedMicros(300, _last_time)) {
            _state = BRING_DATA_LOW;
         }
         break;
      case BRING_DATA_LOW:
         setLow(_pin_data);
         _last_time = micros();
         _state = WAIT3;
         break;
      case WAIT3:
         if (elapsedMicros(10, _last_time)) {
            _state = START_BIT;
         }
         break;
      case START_BIT:
         // Release the clock line
         setHigh(_pin_clk);
         _state = TX_WAIT_CLK_FALL;
         break;
      case TX_WAIT_CLK_FALL:
         // Wait for the PS2 device to take control of the clock line
         if (digitalRead(_pin_clk) == LOW) {
            _bit_cnt = 0;
            _state = TX_BIT;
         }
         break;
      case TX_BIT:
         // Send data bit
         if (_tx_data & 0x01) {
            setHigh(_pin_data);
         } else {
            setLow(_pin_data);
         }
         _state = TX_WAIT_CLK_HIGH;
         break;
      case TX_WAIT_CLK_HIGH:
         if (digitalRead(_pin_clk) == HIGH) {
            _state = TX_WAIT_CLK_LOW;
         }
         break;
      case TX_WAIT_CLK_LOW:
         if (digitalRead(_pin_clk) == LOW) {
            _parity = _parity ^ (_tx_data & 0x01);
            _tx_data = _tx_data >> 1;
            _bit_cnt += 1;
            if (_bit_cnt < 8) {
               _state = TX_BIT;
            } else {
               _state = PARITY_BIT;
            }
         }
         break;
      case PARITY_BIT:
         if (_parity) {
            setHigh(_pin_data);
         } else {
            setLow(_pin_data);
         }
         _state = PAR_WAIT_CLK_HIGH;
         break;
      case PAR_WAIT_CLK_HIGH:
         if (digitalRead(_pin_clk) == HIGH) {
            _state = PAR_WAIT_CLK_LOW;
         }
         break;
      case PAR_WAIT_CLK_LOW:
         if (digitalRead(_pin_clk) == LOW) {
            _state = STOP_BIT;
         }
         break;
      case STOP_BIT:
         setHigh(_pin_data);
         _last_time = micros();
         _state = WAIT4;
         break;
      case WAIT4:
         // Instead of waiting a fixed time can we wait for data low instead? (as in step 10 of http://www.computer-engineering.org/ps2protocol/)
         if (elapsedMicros(50, _last_time)) {
            _state = STOP_WAIT_CLK_LOW;
         }
         break;
      case STOP_WAIT_CLK_LOW:
         if (digitalRead(_pin_clk) == LOW) {
            _state = WAIT_MODE_SWITCH;
         }
         break;
      case WAIT_MODE_SWITCH:
         if ((digitalRead(_pin_clk) == HIGH) && (digitalRead(_pin_data) == HIGH)) {
            // Hold up incoming data until next request
            setLow(_pin_clk);
            // Transmit ended
            _busy = 0;
            // Goto idle state
            _state = IDLE_STATE;
         }
         break;
      // === READ BYTE ===
      case START_READ:
         // Start clock
         setHigh(_pin_clk);
         setHigh(_pin_data);
         _last_time = micros();
         _state = WAIT5;
         break;
      case WAIT5:
         if (elapsedMicros(50, _last_time)) {
            _state = RX_WAIT_CLK_FALL;
         }
         break;
      case RX_WAIT_CLK_FALL:
         if (digitalRead(_pin_clk) == LOW) {
            _last_time = micros();
            _state = WAIT6;
         }
         break;
      case WAIT6:
         if (elapsedMicros(5, _last_time)) {
            _state = RX_START_BIT;
         }
         break;
      case RX_START_BIT:
         if (digitalRead(_pin_clk) == HIGH) {
            _bit_cnt = 0;
            _rx_data = 0;
            _state = RX_WAIT_CLK_LOW;
         }
         break;
      case RX_WAIT_CLK_LOW:
         if (digitalRead(_pin_clk) == LOW) {
            _state = RX_BIT;
         }
         break;
      case RX_BIT:
         // Read data bit
         if (digitalRead(_pin_data) == HIGH) {
            bit = 0x01 << _bit_cnt;
            _rx_data = _rx_data | bit;
         }
         _state = RX_WAIT_CLK_HIGH;
         break;
      case RX_WAIT_CLK_HIGH:
         if (digitalRead(_pin_clk) == HIGH) {
            _bit_cnt += 1;
            if (_bit_cnt < 8) {
               _state = RX_WAIT_CLK_LOW;
            } else {
               _state = PARITY_BIT;
            }
         }
         break;
      case RX_PAR_WAIT_CLK_LOW:
         if (digitalRead(_pin_clk) == LOW) {
            _state = RX_PAR_WAIT_CLK_HIGH;
         }
         break;
      case RX_PAR_WAIT_CLK_HIGH:
         if (digitalRead(_pin_clk) == HIGH) {
            // Receiving parity bit, but ignore it
            _state = RX_STOP_WAIT_CLK_LOW;
         }
         break;
      case RX_STOP_WAIT_CLK_LOW:
         if (digitalRead(_pin_clk) == LOW) {
            _state = RX_STOP_WAIT_CLK_HIGH;
         }
         break;
      case RX_STOP_WAIT_CLK_HIGH:
         if (digitalRead(_pin_clk) == HIGH) {
            // Receiving stop bit
            // Hold up incoming data until next request
            setLow(_pin_clk);
            // Receive ended
            _busy = 0;
            // Goto idle state
            _state = IDLE_STATE;
         }
         break;
   } // end switch
   Serial.print("STATE = ");
   Serial.println(_state, DEC);
}
