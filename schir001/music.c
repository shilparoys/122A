/* Partner 1 Name & E-mail: Kyle Grout (kgrou001@ucr.edu)
 * Partner 2 Name & E-mail: Shilpa Chirackel (schir001@ucr.edu)
 * Lab Section: 221
 * Assignment: Lab 8 Exercise 1
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}

unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}
// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		// prevents OCR3A from underflowing, using prescaler 64
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum SND_States {SND_Init, Snd_Wait, SND_C4, SND_D4, SND_E4} SND_State;
unsigned char button = 0x00;
double freq = 0;

void tick_fct(){
	button = ~PINA & 0x07;
	switch(SND_State){
		case SND_Init:
			SND_State = Snd_Wait;
			break;
		case Snd_Wait:
			if(GetBit(button, 0) && !GetBit(button,1) && !GetBit(button,2)){
				SND_State = SND_C4;
			}
			else if(!GetBit(button, 0) && GetBit(button,1) && !GetBit(button,2)){
				SND_State = SND_D4;
			}
			else if(!GetBit(button, 0) && !GetBit(button,1) && GetBit(button,2)){
				SND_State = SND_E4;
			}
			else{
				SND_State = Snd_Wait;
			}
			break;
		case SND_C4:
			if(GetBit(button, 0) && !GetBit(button,1) && !GetBit(button,2)){
				SND_State = SND_C4;
			}
			else{
				SND_State = Snd_Wait;
			}
			break;
		case SND_D4:
			if(!GetBit(button, 0) && GetBit(button,1) && !GetBit(button,2)){
				SND_State = SND_D4;
			}
			else{
				SND_State = Snd_Wait;
			}
			break;
		case SND_E4:
			if(!GetBit(button, 0) && !GetBit(button,1) && GetBit(button,2)){
				SND_State = SND_E4;
			}
			else{
				SND_State = Snd_Wait;
			}
			break;
		default:
			SND_State = SND_Init;
			break;
	}
	switch(SND_State){
		case SND_Init:
			freq = 0;
			set_PWM(freq);
			break;
		case Snd_Wait:
			freq = 0;
			set_PWM(freq);
			break;
		case SND_C4:
			freq = 261.63;
			set_PWM(freq);
			break;
		case SND_D4:
			freq = 293.66;
			set_PWM(freq);
			break;
		case SND_E4:
			freq = 329.63;
			set_PWM(freq);
			break;
		default:
			freq = 0;
			set_PWM(freq);
			break;
	}
}

int main(void){
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	SND_State = SND_Init;
	PWM_on();
	while(1){
		tick_fct();
	}
	return 0;
}