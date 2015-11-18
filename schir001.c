//header files
#include <avr/io.h>
#include "bit.h"
#include "keypad.h"
#include "lcd.h"
#include "scheduler.h"
#include <stdlib.h>

int i = 0;					//password count counter
unsigned char btn;			//keypad press
unsigned char array[4];		//password array
int num = 0;				//array index
int password = 0;			//user entered password
unsigned char go = 0;		//control to exit pswd_wait
unsigned char isValid = 0;	//user entered correct password or not 
int cursorIndex = 12;		//cursor index
//hardcoded passwords 
int masterP1 = 1234;		
int masterP2 = 5112;
int masterp3 = 9727;


enum masterStates {pswd_wait, pswd_read, checkPwd, door, unlock, lock };
int TickFct_master( int state ) {
	switch( state ) {
		case -1:
			state = pswd_wait;
			break;
		case pswd_wait:
			btn = GetKeypadKey();
			if (btn != '\0' && btn != 'A' && btn != 'B' && btn != 'C' && btn != 'D' && go == 0 ){
				state = pswd_read;
			}
			else if (go == 1){
				go = 0;
				state = checkPwd;
			}
			else
				state = pswd_wait;
			break;
		case pswd_read:
			state = pswd_wait;
			break;
		case checkPwd:
			if(!isValid){
				LCD_ClearScreen();
				state = pswd_wait;
			}
			else{
				LCD_ClearScreen();
				state = door;
			}
			break;
		case door:
			btn = GetKeypadKey();
			if (btn == '3'){
				state = unlock;
			}
			else if(btn == '6'){
				state = lock;
			}
			else{
				state = door;
			}
			break;
		case lock:
			break;
		case unlock:
			break;
		default:
			break;
	}
	switch( state ) {
		case pswd_wait:
			LCD_DisplayString1(1, "Enter Pwd: ");
			break;
		case pswd_read:
			LCD_Cursor(cursorIndex++);
			LCD_WriteData('*');
			array[num++] = btn;
			i++;
			if(i == 4){
				cursorIndex = 12;
				go = 1;
				i = 0;
				num = 0;
			}
			break;
		case checkPwd:
			LCD_ClearScreen();
			password = atoi(array);
			if(password == masterP1 || password == masterP2 || password == masterp3){
				LCD_DisplayString(1, "Correct Pwd");
				isValid = 1;
			}
			else{
				LCD_DisplayString(1, "Incorrect Pwd");
				memset(&array[0], 0, sizeof(array));
				isValid = 0;
			}
			break;
		case door:
			LCD_DisplayString1(1, "Unlock: 3");
			LCD_DisplayString1(17,"Lock:   6");
			break;
		case lock:
			LCD_ClearScreen();
			LCD_DisplayString1(1, "Lock");
			break;
		case unlock:
			LCD_ClearScreen();
			LCD_DisplayString1(1, "Unlock");
			break;
		default:
			break;
	}
	
	return state;
}



int main(void)
{
	DDRC = 0xF0; PORTC = 0x0F;//keypad 
	DDRD = 0xFF; PORTD = 0x00;//lcd data lines
	DDRA = 0xFF; PORTA = 0x00;//lcd control lines
	LCD_init();
	
	tasksNum = 1; // declare number of tasks
	task tsks[tasksNum]; // initialize the task array
	tasks = tsks; // set the task array
	
	// define tasks
	unsigned char i=0; // task counter
	tasks[i].state = -1;
	tasks[i].period = 20;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_master;
	
	TimerSet(100);
	TimerOn();
	
	
	while(1)
	{
		
	}
}