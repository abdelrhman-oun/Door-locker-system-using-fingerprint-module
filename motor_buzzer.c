/*
 * motor.c
 *
 *  Created on: Jul 12, 2021
 *      Author: getra
 */


#include "motor_buzzer.h"



//PD2,PD3: motor outputs, PA7: motor enable.
void Motor_init(void){
	DDRD|=(1<<PD2)|(1<<PD3);
	DDRA|=(1<<PA7);
	CLEAR_BIT(PORTD,PD2);
	CLEAR_BIT(PORTD,PD3);
	CLEAR_BIT(PORTA,PA7);
}


void Motor_cw(void){
	SET_BIT(PORTD,PD2);
	CLEAR_BIT(PORTD,PD3);
	SET_BIT(PORTA,PA7);
}


void Motor_acw(void){
	CLEAR_BIT(PORTD,PD2);
	SET_BIT(PORTD,PD3);
	SET_BIT(PORTA,PA7);
}

void Motor_hold(void){
	CLEAR_BIT(PORTD,PD2);
	CLEAR_BIT(PORTD,PD3);
	CLEAR_BIT(PORTA,PA7);
}


void Motor_stop(void){
	CLEAR_BIT(PORTD,PD2);
	CLEAR_BIT(PORTD,PD3);
	CLEAR_BIT(PORTA,PA7);
}




//PB3: buzzer output.
void buzzer_init(void){
	SET_BIT(DDRB,PB3);
	CLEAR_BIT(PORTB,PB3);
}


void buzzer_start(void){
	SET_BIT(PORTB,PB3);
}


void buzzer_stop(void){
	CLEAR_BIT(PORTB,PB3);
}
