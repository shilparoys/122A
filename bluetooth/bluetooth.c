/*
 * amalu001_lcd_keypad.c
 * prototype 1
 * Created: 11/18/2015 10:31:58 AM
 *  Author: Alyza Malunao
 */ 


#include <avr/io.h>

#include "scheduler.h"
#include "usart_ATmega1284.h"

unsigned char go = 0;					//global variable
unsigned char i = 0;					//count button presses
unsigned char cnt = 0;					//counter for lcd display
unsigned char isPasswordValid = -1;		//bool isPasswordValid,1 valid; isPasswordValid, 0 invalid
unsigned char setPassword[4] = {'1','2','3','4'};
unsigned char password[4];				//user input password
unsigned char input;					//init with GetKeypad()
int cursor = 10;						//cursor for lcd display
unsigned char isLock = -1;				//bool isLock,1 lock; isLock,0 unlock
unsigned char data = 0x00;				//char to send with USART to uc2
unsigned char isSent = 0;



enum menuState {send};
int MenuFct(int state)
{
	switch(state)
	{
		
		case send:
			{
				state = send;
			}
			break;

	}
	switch(state)
	{
		
		case send:
			if(USART_IsSendReady(0)){
				USART_Send('R', 0);
				
			}

			break;
	
	}
	return state;
}

int main(void)
{
	initUSART(0);
	USART_Flush(0);
	


	
	tasksNum = 1;
	task function[tasksNum];
	tasks = function;
	
	unsigned char a=0; //task counter

	
	tasks[a].state = send;
	tasks[a].period = 150;
	tasks[a].elapsedTime = tasks[a].period;
	tasks[a].TickFct = &MenuFct;
	
	TimerSet(150);
	TimerOn();
	

	
    while(1)
    {}
}