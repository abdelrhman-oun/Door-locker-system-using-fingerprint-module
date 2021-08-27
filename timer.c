 /******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.c
 *
 * Description: Source file for the Timer driver
 *
 * Author: Ahmed Adel
 *
 *******************************************************************************/

#include "timer.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

static volatile void (*t0_ovf_callBackPtr)(void) = NULL_PTR;
static volatile void (*t0_comp_callBackPtr)(void) = NULL_PTR;
static volatile void (*t1_ovf_callBackPtr)(void) = NULL_PTR;
static volatile void (*t1_comp_callBackPtr)(void) = NULL_PTR;
static volatile void (*t2_ovf_callBackPtr)(void) = NULL_PTR;
static volatile void (*t2_comp_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect)
{
	if(t0_ovf_callBackPtr != NULL_PTR)
	{
		//call the function using pointer to function
		(*t0_ovf_callBackPtr)();
	}
}

ISR(TIMER0_COMP_vect)
{
	if(t0_comp_callBackPtr != NULL_PTR)
	{
		//call the function using pointer to function
		(*t0_comp_callBackPtr)();
	}
}

ISR(TIMER1_OVF_vect)
{
	if(t1_ovf_callBackPtr != NULL_PTR)
	{
		//call the function using pointer to function
		(*t1_ovf_callBackPtr)();
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(t1_comp_callBackPtr != NULL_PTR)
	{
		//call the function using pointer to function
		(*t1_comp_callBackPtr)();
	}
}

ISR(TIMER2_OVF_vect)
{
	if(t2_ovf_callBackPtr != NULL_PTR)
	{
		//call the function using pointer to function
		(*t2_ovf_callBackPtr)();
	}
}

ISR(TIMER2_COMP_vect)
{
	if(t2_comp_callBackPtr != NULL_PTR)
	{
		//call the function using pointer to function
		(*t2_comp_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/* Description : Function to initialize the timer driver
 * 	1. initialize the required timer registers
 * 	2. set the required clock
 * 	3. set the required mode
 * 	4. set the required interrupts */
void timer_init(Timer_configType * Config_Ptr){
	//switch case to check which timer to be enabled
	switch (Config_Ptr -> num){
	case timer0:
		//setting the timer mode and clock
		TCCR0 = (1 << FOC0) | ((Config_Ptr -> mode) << WGM01) | ((Config_Ptr -> clock) << CS00);
		//set the start value of the timer
		TCNT0 = ((uint8)(Config_Ptr -> start_value));
		//set the compare value of the timer
		OCR0 = ((uint8)(Config_Ptr -> compare_value));
		//enable required interrupts (masking for not messing with other timer interrupts)
		TIMSK = (TIMSK & 0xFC) | (((Config_Ptr -> interrupt)&0x03) << TOIE0);
		break;
	case timer1:
		TCCR1A = (1 << FOC1A) | (1 << FOC1B);
		//setting the timer mode and clock
		TCCR1B = ((Config_Ptr -> mode) << WGM12) | ((Config_Ptr -> clock) << CS10);
		//set the start value of the timer
		TCNT1 = (Config_Ptr -> start_value);
		//set the compare value of the timer
		OCR1A = (Config_Ptr -> compare_value);
		//enable required interrupts (masking for not messing with other timer interrupts)
		TIMSK =(TIMSK & 0xC3) | (((Config_Ptr -> interrupt)&0x05) << TOIE1);

		break;
	case timer2:
		//setting the timer mode and clock
		TCCR2 = (1 << FOC2) | ((Config_Ptr -> mode) << WGM21) | ((Config_Ptr -> clock) << CS20);
		//set the start value of the timer
		TCNT2 = ((uint8)(Config_Ptr -> start_value));
		//set the compare value of the timer
		OCR2 = ((uint8)(Config_Ptr -> compare_value));
		//enable required interrupts (masking for not messing with other timer interrupts)
		TIMSK = (TIMSK & 0x3F) | (((Config_Ptr -> interrupt)&0x03) << TOIE2);
		break;
	}
}

/* Description : Function to stop the timer's clock */
void timer_stop(Timer_configType * Config_Ptr){
	switch (Config_Ptr -> num){
	case timer0:
		TCCR0 &= ~(1<<CS02) & ~(1<<CS01) & ~(1<<CS00);
		break;
	case timer1:
		TCCR1B &= ~(1<<CS12) & ~(1<<CS11) & ~(1<<CS10);
		break;
	case timer2:
		TCCR2 &= ~(1<<CS22) & ~(1<<CS21) & ~(1<<CS20);
		break;
	}
}

/* Description : Function to set timer's clock to required value and resume the timer */
void timer_start(Timer_configType * Config_Ptr){
	switch (Config_Ptr -> num){
	case timer0:
		TCCR0 |= ((Config_Ptr -> clock) << CS00);
		break;
	case timer1:
		TCCR1B |= ((Config_Ptr -> clock) << CS00);
		break;
	case timer2:
		TCCR2 |= ((Config_Ptr -> clock) << CS00);
		break;
	}
}

/* Description : Function to clear the timer's value */
void timer_clearValue(Timer_configType * Config_Ptr){
	switch (Config_Ptr -> num){
	case timer0:
		TCNT0 = 0;
		break;
	case timer1:
		TCNT1 = 0;
		break;
	case timer2:
		TCNT2 = 0;
		break;
	}
}

/* Description : Function to get the timer's value */
uint16 timer_getValue(Timer_configType * Config_Ptr){
	uint16 value;
	switch (Config_Ptr -> num){
	case timer0:
		//read the timer's value
		value = TCNT0;
		break;
	case timer1:
		//read the timer's value
		value = TCNT1;
		break;
	case timer2:
		//read the timer's value
		value = TCNT2;
		break;
	}
	return value;
}

/* Description : Functions to set the callback function address */
void t0_ovf_setCallBack(void(*a_ptr)(void)){
	/* Save the address of the Call back function in a global variable */
	t0_ovf_callBackPtr = a_ptr;
}

void t0_comp_setCallBack(void(*a_ptr)(void)){
	/* Save the address of the Call back function in a global variable */
	t0_comp_callBackPtr = a_ptr;
}

void t1_ovf_setCallBack(void(*a_ptr)(void)){
	/* Save the address of the Call back function in a global variable */
	t1_ovf_callBackPtr = a_ptr;
}

void t1_comp_setCallBack(void(*a_ptr)(void)){
	/* Save the address of the Call back function in a global variable */
	t1_comp_callBackPtr = a_ptr;
}

void t2_ovf_setCallBack(void(*a_ptr)(void)){
	/* Save the address of the Call back function in a global variable */
	t2_ovf_callBackPtr = a_ptr;
}

void t2_comp_setCallBack(void(*a_ptr)(void)){
	/* Save the address of the Call back function in a global variable */
	t2_comp_callBackPtr = a_ptr;
}

/* Description : Function to disable the required timer */
void timer_deInit(Timer_configType * Config_Ptr){
	switch (Config_Ptr -> num){
		case timer0:
			/* Clear All Timer1 Registers */
			TCCR0 = 0;
			TCNT0 = 0;
			OCR0 = 0;
			TIMSK = (TIMSK & 0xFC);
			break;
		case timer1:
			/* Clear All Timer1 Registers */
			TCCR1A = 0;
			TCCR1B = 0;
			TCNT1 = 0;
			OCR1A = 0;
			TIMSK =(TIMSK & 0xC3);
			break;
		case timer2:
			/* Clear All Timer1 Registers */
			TCCR2 = 0;
			TCNT2 = 0;
			OCR2 = 0;
			ASSR = 0;
			TIMSK = (TIMSK & 0x3F);
			break;
	}
}
