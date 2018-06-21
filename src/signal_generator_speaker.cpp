/* Generates a square wave signal with frequency 500 Hz on pin 11, using Timer0 on ATmega32U4 (8bit).
 * Starts/stops playing when triggered by an outside event. */

#include "signal_generator_speaker.h"
#include "signal_generator_timer.h"
#include "Arduino.h"
#include "config.h"

void SGSpeakerSetup(){
	pinMode(speakerPin, OUTPUT);

 	noInterrupts();		// disable all interrupts
 	TCCR0A = 0;
 	TCCR0B = 0;
 	TCNT0 = 0;

	// Timer0 - CTC mode and prescaler = 64
	TCCR0A |= (1 << WGM01);
	TCCR0A &= ~(1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00);

	// Compare output mode: toggle OC0A on compare match
	TCCR0A |= (1 << COM0A0);
	TCCR0A &= ~(1 << COM0A1);

	OCR0A = 250;		// 16MHz/(ps*f*2), ps = 64, f = 500Hz

	interrupts();		// enable all interrupts
}


void SGSpeakerPlaySound(){
	static int sound_playing = 0;
	if (!sound_playing){
		// Start timer by setting prescaler to 64
		TCCR0B |= (1 << CS01) | (1 << CS00);
    	sound_playing = 1;
	}
	else{
		// Stop timer by clearing prescaler
		TCCR0B &= ~(1 << CS02) & ~(1 << CS01) & ~(1 << CS00);
		TCNT0 = 0;
    	sound_playing = 0;
	}
}

