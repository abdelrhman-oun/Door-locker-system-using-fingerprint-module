/*
 * Main_app_ya_rab.c
 *
 *  Created on: Jul 12, 2021
 *      Author: Ahmed Adel
 */



#include "EEPROM.h"
#include "motor_buzzer.h"
#include "lcd.h"
#include "keypad.h"
#include "timer.h"
#include "init_structure.h"



#define PASS_STATE_ADDRESS 0x5F
#define FINGERPRINT_STATE_ADDRESS 0x6F
#define FINGERPRINT_COUNT_ADDRESS 0X7F
#define PASS_ADDRESS 0x8F
#define PASS_LENGTH 4
#define PRESS_ENTER '#'
#define THIEF (2u)



void Get_PW(uint8 * pass_ptr);
void PW_confirm(uint8 * pass1_ptr, uint8 * pass2_ptr);
void PW_save(uint8 * pass_ptr);
void PW_compare(uint8 * pass_ptr,uint8 * status);
void timer_tickCount(void);
void open_door(void);
void alarm_start(void);
void arduino_init(void);
uint8 Fing_match(void);
void Add_fingerPrint(void);
void Remove_fingerPrint(void);
void RemoveAll_fingerPrint(void);



typedef enum {
	pw_not_entered=1,pw_entered=2
}Pass_state;
typedef enum {
	print_not_entered=1,print_entered=2,print_not_required=3
}Fingerprint_state;



uint8 pass_confirm=0; //flag for pw confirmation
uint8 timer_tick=0;
Pass_state pass_state = pw_not_entered;
Fingerprint_state print_state = print_not_entered;
uint8 print_count=0;

int main(void){
	uint8 pass1[PASS_LENGTH],pass2[PASS_LENGTH];

	/* 1.status: to check the status of the entered PW if it matches the saved one
	 * 2.choice: to save the choice of the user to open the door or change PW
	 * */
	uint8 status,choice=0,choice_select=0;
	uint8 print_match=10,not_match_count=0;

	sei();
	//SREG |= (1<<7);//enable interrupts


	LCD_init();//LCD initialization
	Motor_init();
	buzzer_init();
	arduino_init();
	timer_init(&timer_config);
	t1_comp_setCallBack(timer_tickCount);



	timer_stop(&timer_config);


				if(EEPROM_readByte(PASS_STATE_ADDRESS)==pw_entered){
					pass_state=pw_entered;
				}
				else{
					EEPROM_writeByte(PASS_STATE_ADDRESS,pass_state);
				}

				if(EEPROM_readByte(FINGERPRINT_STATE_ADDRESS)==print_entered){
					print_state=print_entered;
					print_count=EEPROM_readByte(FINGERPRINT_COUNT_ADDRESS);
				}
				else if(EEPROM_readByte(FINGERPRINT_STATE_ADDRESS)==print_not_required){
					print_state=print_not_required;
					print_count=0;
					EEPROM_writeByte(FINGERPRINT_COUNT_ADDRESS,print_count);
				}
				else{
					EEPROM_writeByte(FINGERPRINT_STATE_ADDRESS,print_state);
				}



	while(1){
		while(pass_state==pw_not_entered){
			LCD_displayStringRowColumn(0,0,"Enter New Pass");
			LCD_goToRowColumn(1,0);
			Get_PW(pass1);//get the PW from user
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"confirm Pass");
			LCD_goToRowColumn(1,0);
			Get_PW(pass2);//get the PW confirmation
			PW_confirm(pass1,pass2);//compare the 2 PWs to confirm
			if(pass_confirm==1){//if the PWs matched send it to the other MC to be saved
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"SUCCESS");
				PW_save(pass1);
				_delay_ms(1000);
			}
		}

		while(print_state==print_not_entered){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"1:Add FP");
			LCD_displayStringRowColumn(1,0,"2:SKIP");
			choice=KeyPad_getPressedKey();
			while(choice != 1 && choice != 2){//wait for the user's choice
				choice=KeyPad_getPressedKey();
			}
			_delay_ms(1000);
			switch (choice){
			case 1:
				Add_fingerPrint();
				EEPROM_writeByte(FINGERPRINT_STATE_ADDRESS,print_state);
				EEPROM_writeByte(FINGERPRINT_COUNT_ADDRESS,print_count);
				break;
			case 2:
				print_state=print_not_required;
				EEPROM_writeByte(FINGERPRINT_STATE_ADDRESS,print_state);
				print_count=0;
				EEPROM_writeByte(FINGERPRINT_COUNT_ADDRESS,print_count);
				break;
			}
		}
		

		if(choice_select==0){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"1:FP 2:PW");
			LCD_displayStringRowColumn(1,0,"3:options");
			choice=KeyPad_getPressedKey();
			while(choice != 1 && choice != 2 && choice != 3){//wait for the user's choice
				choice=KeyPad_getPressedKey();
			}
			choice_select=1;
			_delay_ms(1000);
		}

		switch(choice){
		case 1:
			if(print_state==print_entered){
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"PUT YOUR FINGER");
				print_match=Fing_match();
				while(print_match==NOT_MATCH){
					not_match_count++;
					if(not_match_count<3){
						LCD_clearScreen();
						LCD_displayStringRowColumn(0,0,"ERROR");
						_delay_ms(1000);
						LCD_clearScreen();
						LCD_displayStringRowColumn(0,0,"PUT YOUR FINGER");
						print_match=Fing_match();
					}
					else{
						not_match_count=0;
						choice=2;
						break;
					}
				}
				if(print_match==MATCH){
					not_match_count=0;
					choice_select=0;
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"Welcome");
					open_door();
				}
			}
			else{
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"1:Add FP");
				LCD_displayStringRowColumn(1,0,"2:SKIP");
				choice=KeyPad_getPressedKey();
				while(choice != 1 && choice != 2){//wait for the user's choice
					choice=KeyPad_getPressedKey();
				}
				_delay_ms(1000);
				switch (choice){
				case 1:
					Add_fingerPrint();
					EEPROM_writeByte(FINGERPRINT_STATE_ADDRESS,print_state);
					EEPROM_writeByte(FINGERPRINT_COUNT_ADDRESS,print_count);
					break;
				case 2:
					choice_select=0;
					print_state=print_not_required;
					EEPROM_writeByte(FINGERPRINT_STATE_ADDRESS,print_state);
					print_count=0;
					EEPROM_writeByte(FINGERPRINT_COUNT_ADDRESS,print_count);
					break;
				}
			}
			break;
		case 2:
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Enter Pass");
			_delay_ms(1000);
			LCD_goToRowColumn(1,0);
			Get_PW(pass1);//get PW from user
			PW_compare(pass1, &status);
			while(status==FALSE){
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"ERROR");
				_delay_ms(1000);
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Enter Pass");
				LCD_goToRowColumn(1,0);
				Get_PW(pass1);//get PW from user
				PW_compare(pass1, &status);
			}
			if(status==TRUE){
				choice_select=0;
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Welcome");
				open_door();
			}
			else if(status==THIEF){
				choice_select=0;
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"DOOR LOCKED!!");
				alarm_start();
			}
			break;
		case 3:
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Enter Pass");
			LCD_goToRowColumn(1,0);
			_delay_ms(1000);
			Get_PW(pass1);//get PW from user
			PW_compare(pass1, &status);
			while(status==FALSE){
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"ERROR");
				_delay_ms(1000);
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Enter Pass");
				LCD_goToRowColumn(1,0);
				Get_PW(pass1);//get PW from user
				PW_compare(pass1, &status);
			}
			if(status==TRUE){
				choice_select=0;
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"1:change FP");
				LCD_displayStringRowColumn(1,0,"2:change PW");
				choice=KeyPad_getPressedKey();
				while(choice != 1 && choice != 2){//wait for the user's choice
					choice=KeyPad_getPressedKey();				}
				switch (choice){
				case 1:
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"1:ADD 2:DELETE");
					LCD_displayStringRowColumn(1,0,"3:DELETE ALL");
					_delay_ms(1000);
					choice=KeyPad_getPressedKey();
					while(choice != 1 && choice != 2 && choice != 3){//wait for the user's choice
						choice=KeyPad_getPressedKey();
					}
					_delay_ms(1000);
					switch(choice){
					case 1:
						Add_fingerPrint();
						EEPROM_writeByte(FINGERPRINT_STATE_ADDRESS,print_state);
						EEPROM_writeByte(FINGERPRINT_COUNT_ADDRESS,print_count);
						break;
					case 2:
						LCD_clearScreen();
						LCD_displayStringRowColumn(0,0,"SURE?");
						LCD_displayStringRowColumn(1,0,"1:YES 2:NO");
						_delay_ms(1000);
						choice=KeyPad_getPressedKey();
						while(choice != 1 && choice != 2){//wait for the user's choice
							choice=KeyPad_getPressedKey();
						}
						_delay_ms(1000);
						switch (choice){
						case 1:
							if(print_count!=0){
								Remove_fingerPrint();
								EEPROM_writeByte(FINGERPRINT_STATE_ADDRESS,print_state);
								EEPROM_writeByte(FINGERPRINT_COUNT_ADDRESS,print_count);
							}
							else{
							}
						case 2:
							choice_select=0;
							break;
						}
						break;
					case 3:
						LCD_clearScreen();
						LCD_displayStringRowColumn(0,0,"SURE?");
						LCD_displayStringRowColumn(1,0,"1:YES 2:NO");
						_delay_ms(1000);
						choice=KeyPad_getPressedKey();
						while(choice != 1 && choice != 2){//wait for the user's choice
							choice=KeyPad_getPressedKey();
						}
						switch (choice){
						case 1:
							RemoveAll_fingerPrint();
							EEPROM_writeByte(FINGERPRINT_STATE_ADDRESS,print_state);
							EEPROM_writeByte(FINGERPRINT_COUNT_ADDRESS,print_count);
							break;
						case 2:
							choice_select=0;
							break;
						}
					break;
					}
					break;
				case 2:
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"Enter New Pass");
					_delay_ms(1000);
					LCD_goToRowColumn(1,0);
					Get_PW(pass1);//get the PW from user
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"confirm Pass");
					LCD_goToRowColumn(1,0);
					Get_PW(pass2);//get the PW confirmation
					PW_confirm(pass1,pass2);//compare the 2 PWs to confirm
					while(pass_confirm!=1){//if the PWs matched send it to the other MC to be saved
						LCD_clearScreen();
						LCD_displayStringRowColumn(0,0,"ERROR");
						_delay_ms(1000);
						LCD_displayStringRowColumn(0,0,"Enter New Pass");
						LCD_goToRowColumn(1,0);
						Get_PW(pass1);//get the PW from user
						LCD_clearScreen();
						LCD_displayStringRowColumn(0,0,"confirm Pass");
						LCD_goToRowColumn(1,0);
						Get_PW(pass2);//get the PW confirmation
						PW_confirm(pass1,pass2);//compare the 2 PWs to confirm
					}
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"SUCCESS");
					PW_save(pass1);
					_delay_ms(1000);
					break;
				}
			}
			else if(status==THIEF){
				choice_select=0;
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"DOOR LOCKED!!");
				alarm_start();
			}
			break;
		}
	}
}



/* Description : Function to get the PW from the keypad and save it in an array*/
void Get_PW(uint8 * pass_ptr){
	uint8 i=0;
	for(i=0;i<PASS_LENGTH;i++){
		pass_ptr[i] = KeyPad_getPressedKey(); /* get the pressed button from keypad */
		if((pass_ptr[i] >= 0) && (pass_ptr[i] <= 9)) //check if the user pressed a valid number
		{
			LCD_displayCharacter('#'); //display the password as #
		}
		else{
			i--; //if the user didn't enter a valid number repeat this loop
		}
		_delay_ms(400);
	}
	//wait for user to press enter
	while(KeyPad_getPressedKey() != PRESS_ENTER){
	}
}

/* Description : Function to confirm the PW */
//change name to new pass confirm
void PW_confirm(uint8 * pass1_ptr, uint8 * pass2_ptr){
	uint8 i=0;
	for(i=0;i<PASS_LENGTH;i++){
		if(pass1_ptr[i]!=pass2_ptr[i]){ //compare between the two passwords
			pass_confirm=0; //flag for PW confirmation=0
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"ERROR"); //if the password is wrong display an error
			_delay_ms(1000);
			break;
		}
		else{
			pass_confirm=1; //raise flag for  PW confirmation
		}
	}
}

void PW_save(uint8 * pass_ptr){
	uint8 i=0;
	for(i=0;i<PASS_LENGTH;i++){
		EEPROM_writeByte(PASS_ADDRESS+i,pass_ptr[i]);//save the PW in eeprom
//		_delay_ms(10);
	}
	pass_state=pw_entered;//update PW state
	EEPROM_writeByte(PASS_STATE_ADDRESS,pass_state);//save PW state in eeprom
//	_delay_ms(10);
}

/* Description : Compare the received PW with the already set PW */
void PW_compare(uint8 * pass_ptr,uint8 * status){
	/* 1.wrong: a flag to check if the PW is incorrect
	 * 2.pass_confirm: to read the PW from eeprom and compare it with the entered PW
	 * 3.wrong_count: to count how many times the PW was entered incorrectly */
	uint8 i=0,wrong=0,saved_pass;
	static uint8 wrong_count=0;
	for(i=0;i<PASS_LENGTH;i++){
		saved_pass = EEPROM_readByte(PASS_ADDRESS+i);//read PW from eeprom
//		_delay_ms(10);
		if(pass_ptr[i]!=saved_pass){//compare the PW
			wrong_count++;
			wrong=1;
			if(wrong_count!=3){//if PW is wrong less than 3 times send false status
				*status=FALSE;
				break;
			}
			else{//if PW is wrong for 3 times send thief status and activate the buzzer for 1 min
				*status=THIEF;
				break;
			}
		}
	}
	if(wrong==0){//if PW is correct
		*status=TRUE;
		wrong_count=0;
	}
}

void timer_tickCount(void){
	timer_tick++;
}

void open_door(void){
	timer_start(&timer_config);
	timer_clearValue(&timer_config);
	timer_tick=0;
	Motor_cw();
	//1 tick = 3 sec, so we need to wait for 1 tick
	while(timer_tick<1);
	timer_clearValue(&timer_config);
	timer_tick=0;
	Motor_hold();
	//1 tick = 3 sec, so wait for 3 tick = 9 sec.
	while(timer_tick<3);
	timer_clearValue(&timer_config);
	timer_tick=0;
	Motor_acw();
	//1 tick = 3 sec, so we need to wait for 1 tick1
	while(timer_tick<1);
	timer_stop(&timer_config);
	Motor_stop();
}

void alarm_start(void){
	timer_start(&timer_config);
	timer_clearValue(&timer_config);
	timer_tick=0;
	buzzer_start();
	//20 ticks = 60 sec
	while(timer_tick<20);
	timer_stop(&timer_config);
	buzzer_stop();
}




void arduino_init(void){
	DDRD &= ~(1<<PD0) & ~(1<<PD1);					//PD0 flag, PD1 data
	DDRC |= (1<<PC0) | (1<<PC1) | (1<<PC6);
	PORTC=0;
}





void Add_fingerPrint(void){
	uint8 choice=0,wrong=0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"PUT YOUR FINGER");
	LCD_displayStringRowColumn(1,0,"TWICE");
	CLEAR_BIT(PORTC,PC0);
	CLEAR_BIT(PORTC,PC1);
	SET_BIT(PORTC,PC6);
	_delay_ms(10);
	CLEAR_BIT(PORTC,PC6);
	while(BIT_IS_CLEAR(PIND,PD0));
	if(BIT_IS_SET(PIND,PD1)){
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"SUCCESS");
		if(print_state==print_not_entered||print_state==print_not_required){
			print_state=print_entered;
		}
		print_count++;
		_delay_ms(1000);
		wrong=0;
	}
	else{
		wrong=1;
		while(wrong==1){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"ERROR");
			_delay_ms(1000);
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"1:try again");
			LCD_displayStringRowColumn(1,0,"2:SKIP");
			choice=KeyPad_getPressedKey();
			while(choice != 1 && choice != 2){//wait for the user's choice
				choice=KeyPad_getPressedKey();
			}
			
			switch (choice){
				case 1:
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"PUT YOUR FINGER");
					LCD_displayStringRowColumn(1,0,"TWICE");
					CLEAR_BIT(PORTC,PC0);
					CLEAR_BIT(PORTC,PC1);
					SET_BIT(PORTC,PC6);
					_delay_ms(10);
					while(BIT_IS_CLEAR(PIND,PD0));
					if(BIT_IS_SET(PIND,PD1)){
						LCD_clearScreen();
						LCD_displayStringRowColumn(0,0,"SUCCESS");
						if(print_state==print_not_entered||print_state==print_not_required){
							print_state=print_entered;
						}
						print_count++;
						_delay_ms(1000);
						wrong=0;
					}
					break;
				case 2:
					wrong=0;
					break;	
			}
		}
	}
}


uint8 Fing_match(void){
	CLEAR_BIT(PORTC,PC0);
	SET_BIT(PORTC,PC1);
	SET_BIT(PORTC,PC6);
	_delay_ms(10);
	CLEAR_BIT(PORTC,PC6);
	while(BIT_IS_CLEAR(PIND,PD0));
	if(BIT_IS_SET(PIND,PD1)){
			return 1;
	}
	else{
		return 0;
	}
	}
	
	
	
	void Remove_fingerPrint(void){
	uint8 choice=0,wrong=0;
	SET_BIT(PORTC,PC0);
	SET_BIT(PORTC,PC1);
	SET_BIT(PORTC,PC6);
	_delay_ms(10);
	CLEAR_BIT(PORTC,PC6);
	while(BIT_IS_CLEAR(PIND,PD0));
	if(BIT_IS_SET(PIND,PD1)){
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"SUCCESS");
		print_count--;
		if(print_count==0){
			print_state=print_not_required;
		}
		_delay_ms(1000);
		wrong=0;
	}
	else{
		wrong=1;
		while(wrong==1){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"ERROR");
			_delay_ms(1000);
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"1:try again");
			LCD_displayStringRowColumn(1,0,"2:SKIP");
			choice=KeyPad_getPressedKey();
			while(choice != 1 && choice != 2){//wait for the user's choice
				choice=KeyPad_getPressedKey();
			}
			
			switch (choice){
				case 1:
				SET_BIT(PORTC,PC0);
				SET_BIT(PORTC,PC1);
				SET_BIT(PORTC,PC6);
				_delay_ms(10);
				while(BIT_IS_CLEAR(PIND,PD0));
				if(BIT_IS_SET(PIND,PD1)){
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"SUCCESS");
					print_count--;
					if(print_count==0){
						print_state=print_not_required;
					}
					_delay_ms(1000);
					wrong=0;
				}
				break;
				case 2:
				wrong=0;
				break;
			}
		}
	}
}
	
	
	
	
	
	
	
	
	void RemoveAll_fingerPrint(void){
	uint8 choice=0,wrong=0;
	SET_BIT(PORTC,PC0);
	CLEAR_BIT(PORTC,PC1);
	SET_BIT(PORTC,PC6);
	_delay_ms(10);
	CLEAR_BIT(PORTC,PC6);
	while(BIT_IS_CLEAR(PIND,PD0));
	if(BIT_IS_SET(PIND,PD1)){
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"SUCCESS");
		print_state=print_not_required;
		print_count=0;
		_delay_ms(1000);
		wrong=0;
	}
	else{
		wrong=1;
		while(wrong==1){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"ERROR");
			_delay_ms(1000);
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"1:try again");
			LCD_displayStringRowColumn(1,0,"2:SKIP");
			choice=KeyPad_getPressedKey();
			while(choice != 1 && choice != 2){//wait for the user's choice
				choice=KeyPad_getPressedKey();
			}
			
			switch (choice){
				case 1:
				SET_BIT(PORTC,PC0);
				CLEAR_BIT(PORTC,PC1);
				SET_BIT(PORTC,PC6);
				_delay_ms(10);
				while(BIT_IS_CLEAR(PIND,PD0));
				if(BIT_IS_SET(PIND,PD1)){
					LCD_clearScreen();
					LCD_displayStringRowColumn(0,0,"SUCCESS");
					print_state=print_not_required;
					print_count=0;
					_delay_ms(1000);
					wrong=0;
				}
				break;
				case 2:
				wrong=0;
				break;
			}
		}
	}

	}