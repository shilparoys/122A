//header files
#include <avr/io.h>
#include "bit.h"
#include "keypad.h"
#include "lcd.h"
#include "scheduler.h"
#include <stdlib.h>
#include "usart_ATmega1284.h"

int i = 0;					//password count counter
unsigned char btn;			//keypad press
unsigned char array[4];		//password array
int num = 0;				//array index
int password = 0;			//user entered password
unsigned char go = 0;		//control to exit pswd_wait
unsigned char isValid = 0;	//user entered correct password or not 
int cursorIndex = 12;		//cursor index
char motor_arr[] = {0x11, 0x33, 0x22, 0x66, 0x44, 0xCC, 0x88, 0x99};	//motor array
int position = 0;
int numPhases = 0;
unsigned char go1 = 0;
unsigned char toLock = '2';
//0 is unlock and 1 is lock
unsigned char isLock = '1';
unsigned char test = 0;
int wrongPass = 0;
unsigned char numWrongPass = '0';
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
			//avoid invalid key press
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
			if (btn == '3' && isLock == '1'){
				state = unlock;
			}
			else if(btn == '6' && (isLock == '0')){
				state = lock;
			}
			else{
				state = door;
			}
			break;
		case lock:
			toLock = '1';
			isLock = '1';
			state = pswd_wait;
			break;
		case unlock:
			toLock = '0';
			isLock = '0';
			state = pswd_wait;
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
				++wrongPass;
				if((wrongPass % 3) == 0){
					LCD_DisplayString1(1, "BAD");
					numWrongPass = '1';
				}
			}
			break;
		case door:
			LCD_DisplayString1(1, "Unlock: 3");
			LCD_DisplayString1(17,"Lock:   6");
			//LCD_WriteData(isLock);
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
unsigned char menu1[] = "Intruder Alert\n";
int i1 = 0;
int i2 = 0;
int num1 = 0;
unsigned char canRead = 0;

enum send_data{wait_send};
int send_data(int state){
	switch(state){
		case -1:
			if(numWrongPass == '1')
				state = wait_send;
			else
				state = -1;
			break;
		case wait_send:
			break;
		default:
			break;
	}
	switch(state){
		case -1:
			break;
		case wait_send:
			if(USART_IsSendReady(0) && i1 < 15){
				USART_Send(menu1[i1], 0);
				i1++;
			}
			break;
		default:
			break;
	}
	return state;
}

enum read_data{init_data, readPassword, checkPassword};
int read_data(int state){
	switch(state){
		case init_data:
			if(canRead == '1'){
				state = readPassword;
			}
			else{
				state = init_data;
			}
			break;
		case readPassword:
			state = checkPassword;
			break;
		case checkPassword:
			break;
		default:
			break;
	}
	switch(state){
		case init_data:
			break;
		case readPassword:
		 if(USART_HasReceived(0)){
				test = USART_Receive(0);
				array[num++] = test;
		 		USART_Flush(0);
		 		LCD_WriteData(test);
		 		i2++;
		 	}
		 break;
		 case checkPassword:
		 	break;
		 default:
		 	break;

	}
	return state;
}
enum State{receive, input, cw, ccw, stop};
int motor(int state)
{
	switch(state)
	{
		case -1:
			state = receive;
			break;
		case receive:
			state = input;
			break;
		case input:
			if(toLock == '1'){
				numPhases = (90/5.625) * 64;
				toLock = '2';
				state = ccw;
			}
			else if(toLock == '0'){
				numPhases = (90/5.625)*64;
				toLock = '2';
				state = cw;
			}
			else
				state = input;
		break;
		case cw:
			if(go1 == 1){
				go1 = 0;
				state = input;
			}
			else{
				state = cw;
			}
			break;
		case ccw:
			if(go1 == 1){
				go1 = 0;
				state = input;	
			}
			else{
				state = ccw;
			}	
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
		case receive:
			break;
		case input:
			break;
		case cw:
			if(position < 7 && numPhases > 0){
				numPhases--;
				position++;
			}
			else
			position = 0;
			if(numPhases <= 0){
				go1 = 1;
			}
			
			break;
		
		case ccw:
			if(position > 0 && numPhases > 0){
				numPhases--;
				position--;
			}
			else
			position = 7;
			if(numPhases <= 0){
				go1 = 1;
			}
			
		
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
	initUSART(0);	//initialize USART (0)
	USART_Flush(0);
	
	//initalize ports
	DDRC = 0xF0; PORTC = 0x0F;//keypad 
	DDRB = 0xFF; PORTB = 0x00;//lcd data lines
	DDRD = 0xFF; PORTD = 0x00;//lcd control lines
	PORTA = 0x00; DDRA = 0xFF;//stepper motor
	
	LCD_init();	//LCD initialize
	
	tasksNum = 3; // declare number of tasks
	task tsks[tasksNum]; // initialize the task array
	tasks = tsks; // set the task array
	
	// define tasks
	unsigned char i = 0; // task counter

	//bluetooth
	tasks[i].state = -1;
	tasks[i].period = 150;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &send_data;
	i++;
	/*tasks[i].state = init_data;
	tasks[i].period = 150;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &read_data;
	i++;*/
	//password
	tasks[i].state = -1;
	tasks[i].period = 200;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_master;
	i++;
	//motor
	tasks[i].state = -1;
	tasks[i].period = 3;
	tasks[i].elapsedTime = tasks[0].period;
	tasks[i].TickFct = &motor;

	TimerSet(1);
	TimerOn();
	
	
	while(1)
	{}
}
