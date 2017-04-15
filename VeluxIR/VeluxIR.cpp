#include "VeluxIR.h"
#include "Arduino.h"

//#define MARK_SHORT 320 // us
//#define MARK_LONG 1100
//#define SPACE_SHORT 380
//#define SPACE_LONG 1220
#define MARK_SHORT 460 // us
#define MARK_LONG 1400
#define SPACE_SHORT 485
#define SPACE_LONG 1275

#define REPEAT_DELAY 20  // ms
#define DEBOUNCE_DELAY 500 // ms

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
      pulseIR(MARK_LONG);
      delayMicroseconds(MARK_SHORT);
    }
    else {
      pulseIR(SPACE_SHORT);
      delayMicroseconds(SPACE_LONG);
    }
    mask >>= 1;
  }

}
