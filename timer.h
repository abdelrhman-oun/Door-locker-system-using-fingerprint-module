 /******************************************************************************
 *
 * Module: Timer
 *
 * File Name: timer.h
 *
 * Description: Header file for the Timer driver
 *
 * Author: Ahmed Adel
 *
 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"
#include "common_macros.h"
#include "micro_config.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum{
	timer0,timer1,timer2
}Timer_Num;
typedef enum{
	m_normal,m_ctc
}Timer_Mode;
typedef enum{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024,EXT_T0_FALL,EXT_T0_RISE
}Timer_Clock;
typedef enum{
	i_off,i_normal,i_ctc=6
}Timer_Int;

typedef struct{
	Timer_Num num;
	Timer_Mode mode;
	Timer_Clock clock;
	Timer_Int interrupt;
	uint16 start_value;
	uint16 compare_value;
}Timer_configType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void timer_init(Timer_configType * Config_Ptr);
void timer_stop(Timer_configType * Config_Ptr);
void timer_start(Timer_configType * Config_Ptr);
void timer_clearValue(Timer_configType * Config_Ptr);
uint16 timer_getValue(Timer_configType * Config_Ptr);
void t0_ovf_setCallBack(void(*a_ptr)(void));
void t0_comp_setCallBack(void(*a_ptr)(void));
void t1_ovf_setCallBack(void(*a_ptr)(void));
void t1_comp_setCallBack(void(*a_ptr)(void));
void t2_ovf_setCallBack(void(*a_ptr)(void));
void t2_comp_setCallBack(void(*a_ptr)(void));
void timer_deInit(Timer_configType * Config_Ptr);

#endif /* TIMER_H_ */
