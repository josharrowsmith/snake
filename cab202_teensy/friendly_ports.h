/*
*	File:	friendly_ports.h
*	Author: Lawrence Buckingham, Queensland University of Technology.
*	Date:	27 April 2015.
*
*	Some macros to make the manipulation of teensy buttons and LEDs less putrid.
*/

#ifndef __FRIENDLY_PORTS__
#define __FRIENDLY_PORTS__

#include <avr/io.h>

// define the bit mask and register for LED 0 (left)
#define LED0 (1<<2)
#define LED0_PORT PORTB



// define the bit mask and register for LED 1 (right) 
#define LED1 (1<<3)
#define LED1_PORT PORTB

// define the bit mask and register for LED2 
//	(What I'm calling the tiny yellow LED) 
#define LED2 (1<<6)
#define LED2_PORT PORTD

// define the bit mask and register for switch (left button)
#define SW1 (1<<6)
#define SW1_PORT PINF 
// define the bit mask and register for switch (UP button)
#define UP (1<<1)
#define UP_PORT PIND
// define the bit mask and register for switch (DOWN button)
#define DOWN (1<<7)
#define DOWN_PORT PINB
// define the bit mask and register for switch (left button)
#define LEFT (1<<1)
#define LEFT_PORT PINB
// define the bit mask and register for switch (right button)
#define RIGHT (1<<0)
#define RIGHT_PORT PIND



// define the bit mask and register for switch 1 (right)
#define SW2 (1<<5)
#define SW2_PORT PINF

// Turns on a LED, which should be one of LED0, LED1 or LED2
#define turn_on(led) led##_PORT |= (led)

// Turns off a LED, which should be one of LED0, LED1 or LED2
#define turn_off(led) led##_PORT &= ~(led)

// Detects if a particular bitmask is set. The argument should be
// one of SW1 or SW2. The value is non-zero if and only if the button is 
// held down at the time.
#define pressed(button) ((button##_PORT & (button)) != 0)






// Busy-loop wait until a condition becomes non-zero.
// This is intended for debugging only.
#define wait_until( cond ) while ( !cond ) {}

// Turn on left LED, wait for button.
// This is intended for debugging only.
#define db_wait(zzz) { \
	turn_on( LED0 ); \
	turn_on( LED1);\
	wait_until( pressed( SW1 ) ); \
	_delay_ms(100); \
	turn_off( LED0 ); \
}

#endif
