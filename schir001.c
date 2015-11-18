
#include <avr/io.h>
#include "bit.h"
#include "keypad.h"
#include "lcd.h"
#include "scheduler.h"


 unsigned char btn;

enum masterStates {m_read};
int TickFct_master( int state ) {
        switch( state ) {
                case -1:
                        state = m_read;
                        break;
                case m_read:
                        state = m_read;
                        break;
                default:
                        break;          
        }
        
        switch( state ) {
                case m_read:
                        btn = GetKeypadKey();
                        LCD_WriteData(btn);
                  
						//LCD_DisplayString(1,"Ptrn:    Spd:   ");					
											
                        break;
                default:
                        break;
        }
        
        return state;
}



int main(void)
{
        DDRC = 0xF0; PORTC = 0x0F;
        DDRD = 0xFF; PORTD = 0x00;//lcd data lines
        DDRA = 0xFF; PORTA = 0x00;//lcd control lines
		LCD_init();
                
        tasksNum = 1; // declare number of tasks
        task tsks[1]; // initialize the task array
        tasks = tsks; // set the task array
                
        // define tasks
        unsigned char i=0; // task counter
        tasks[i].state = -1;
        tasks[i].period = 100;
        tasks[i].elapsedTime = tasks[i].period;
        tasks[i].TickFct = &TickFct_master;
        
        TimerSet(100);
        TimerOn();
               
        
    while(1)
    {
        
    }
}