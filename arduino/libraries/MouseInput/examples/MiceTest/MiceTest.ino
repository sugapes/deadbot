//==============================================================================
// MiceTest.ino - A small example of the use of the MouseInput library to read
//              from multiple optical mice (PS2 interface)
// Written by Sugapes, August 2014.
// MIT license, check LICENSE file at the top-level directory for more information.
//==============================================================================

#include <MouseInput.h>

#define NR_MICE 4

//==============================================================================
// An arduino sketch to interface with a ps/2 mouse.
// Also uses serial protocol to talk back to the host
// and report what it finds.
//==============================================================================

// Mouse interface pins
MouseInput mouse[NR_MICE];

int display_cnt;
int my_x1, my_x2, my_x3, my_x4;
int my_y1, my_y2, my_y3, my_y4;

void setup()
{
   Serial.begin(9600);
   Serial.println("Program started");

   pinMode(2, INPUT_PULLUP);
   pinMode(3, INPUT_PULLUP);
   pinMode(4, INPUT_PULLUP);
   pinMode(5, INPUT_PULLUP);
   pinMode(6, INPUT_PULLUP);
   pinMode(7, INPUT_PULLUP);
   pinMode(8, INPUT_PULLUP);
   pinMode(9, INPUT_PULLUP);

   mouse[0].init(2, 3); // pin 2 - clock; pin 3 - data
   mouse[1].init(4, 5); // pin 4 - clock; pin 5 - data
   mouse[2].init(6, 7); // pin 6 - clock; pin 7 - data
   mouse[3].init(8, 9); // pin 8 - clock; pin 9 - data

   my_x1 = 0; my_x2 = 0; my_x3 = 0; my_x4 = 0;
   my_y1 = 0; my_y2 = 0; my_y3 = 0; my_y4 = 0;

   display_cnt = 0;
}

// Get a reading from the mouse and report it back to the
// host via the serial line.
void loop()
{
   int i;

   // Retrieve coordinates for all mice
   my_x1 = mouse[0].getX();
   my_x2 = mouse[1].getX();
   my_x3 = mouse[2].getX();
   my_x4 = mouse[3].getX();
   my_y1 = mouse[0].getY();
   my_y2 = mouse[1].getY();
   my_y3 = mouse[2].getY();
   my_y4 = mouse[3].getY();

   // Send the data to the serial console
   if (display_cnt == 100) {
      Serial.print("1: (X=");
      Serial.print(my_x1, DEC);
      Serial.print("\t, Y=");
      Serial.print(my_y1, DEC);
      Serial.print(")\t2: (X=");
      Serial.print(my_x2, DEC);
      Serial.print("\t, Y=");
      Serial.print(my_y2, DEC);
      Serial.print(")\t3:(X=");
      Serial.print(my_x3, DEC);
      Serial.print("\t, Y=");
      Serial.print(my_y3, DEC);
      Serial.print(")\t4: (X=");
      Serial.print(my_x4, DEC);
      Serial.print("\t, Y=");
      Serial.print(my_y4, DEC);
      Serial.print(")");
      Serial.println();
      display_cnt = 0;
   } else {
      display_cnt++;
   }

   // Update state machines
   for (i=0; i < NR_MICE; i++) {
      mouse[i].update();
   }
}
