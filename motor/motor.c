#include <avr/io.h>
#include "scheduler.h"
#include "keypad.h"

char motor_arr[] = {0x11, 0x33, 0x22, 0x66, 0x44, 0xCC, 0x88, 0x99};
int position = 0;
char keypad;
enum State{input, cw, ccw, stop};
int motor(int state)
{
	keypad = GetKeypadKey();
	switch(state)
	{
		case -1:
			state = input;
			break;
		case input:
			if(keypad == '1')
				state = ccw;
			else if(keypad == '2')
				state = cw;
			else
				state = stop;
		break;
		case cw:
			if(keypad != '2')
				state = input;
			else
				state = cw;
			break;
		case ccw:
			if(keypad != '1')
				state = input;
			else
				state = ccw;
			break;
		case stop:
			state = input;
			break;
		default:
			state = input;
			break;
	}
	switch(state)
	{
		case input:

		break;

		case cw:
			if(position < 7)
				position++;
			else
			position = 0;
			
			break;
		
		case ccw:
			if(position > 0)
				position--;
			else
			position = 7;
		
			break;
		case stop:
			position = 0;

			break;
		default:
			break;
	}
	
	PORTA = motor_arr[position];
	
	return state;
}

int main(void)
{
	PORTA = 0x00; DDRA = 0xFF;
	PORTC = 0x0F; DDRC = 0xF0;
	
	tasksNum = 1;
	task function[1];
	tasks = function;
	
	tasks[0].state = -1;
	tasks[0].period = 3;
	tasks[0].elapsedTime = tasks[0].period;
	tasks[0].TickFct = &motor;
	
	TimerSet(1);
	TimerOn();
	
	while(1)
	{}
}