#include "VeluxIR.h"
#include "Arduino.h"

// '0' and '1' durations are asymetric. All durations are in
// pretend microseconds. These durations may be normalized by
// working out the duration of digitalWrite() on a platform - assuming
// delayMicroseconds() is accurate across different hardware.

// We call a '1' a MARK_LONG followed by a SPACE_SHORT
// We call a '0' a MARK_SHORT followed by a SPACE_LONG

// ESP8266
#define MARK_LONG 1441
#define SPACE_SHORT 440 // 1881
#define MARK_SHORT 479
#define SPACE_LONG 1275  // 1754
// ARDUINO UNO
// #define MARK_LONG 1100
// #define SPACE_SHORT 380  // 1480
// #define MARK_SHORT 320
// #define SPACE_LONG 1220 // 1550

#define REPEAT_DELAY 25  // ms

#define PROTOCOL_BITS 24

const unsigned long MOTOR_COMMAND [2][3] = {
  {
    (0b00100100 * 65536) + (0b01100000 * 256) + 0b00001001,
    (0b01100100 * 65536) + (0b01100000 * 256) + 0b00001100,
    (0b10100100 * 65536) + (0b01100000 * 256) + 0b00000011
  },
  {
    (0b00101000 * 65536) + (0b01100000 * 256) + 0b00001100,
    (0b01101000 * 65536) + (0b01100000 * 256) + 0b00001001,
    (0b10101000 * 65536) + (0b01100000 * 256) + 0b00000110
  }
};

int IRledPin = 4;

VeluxIR::VeluxIR() {}

void VeluxIR::init(int pin) {

  IRledPin = pin;

  // initialize the IR digital pin as an output:
  pinMode(IRledPin, OUTPUT);

}

// This procedure sends a 38KHz pulse to the IRledPin
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(short microsecs) {
  // we'll count down from the number of microseconds we are told to wait

  cli();  // this turns off any background interrupts

  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
    digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
    delayMicroseconds(10);         // hang out for 10 microseconds
    digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
    delayMicroseconds(10);         // hang out for 10 microseconds

    // so 26 microseconds altogether
    microsecs -= 26;
  }

  sei();  // this turns them back on
}

void VeluxIR::transmit (short motor, enum command direction) {

  unsigned long code = MOTOR_COMMAND [motor][direction];
  unsigned long mask = pow (2, PROTOCOL_BITS - 1);

  for (int i = 0; i < PROTOCOL_BITS; i++) { //iterate through bit mask
    if (code & mask) {
      // send a '1'
      pulseIR(MARK_LONG);
      delayMicroseconds(SPACE_SHORT);
    }
    else {
      // send a '0'
      pulseIR(MARK_SHORT);
      delayMicroseconds(SPACE_LONG);
    }
    mask >>= 1;
  }

  // The Velux remote pauses and retransmits. Sending once generally
  // works ok but we'll follow slavishly since the Velux micro expacts
  // this behaviour.
  delay (REPEAT_DELAY);

  // reset mask
  mask = pow (2, PROTOCOL_BITS - 1);
  
  for (int i = 0; i < PROTOCOL_BITS; i++) { //iterate through bit mask
    if (code & mask) {
      // send a '1'
      pulseIR(MARK_LONG);
      delayMicroseconds(SPACE_SHORT);
    }
    else {
      // send a '0'
      pulseIR(MARK_SHORT);
      delayMicroseconds(SPACE_LONG);
    }
    mask >>= 1;
  }

}
