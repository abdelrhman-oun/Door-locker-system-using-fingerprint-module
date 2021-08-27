/*
 * motor.h
 *
 *  Created on: Jul 12, 2021
 *      Author: getra
 */

#ifndef MOTOR_BUZZER_H_
#define MOTOR_BUZZER_H_


#include "std_types.h"
#include "common_macros.h"
#include "micro_config.h"


void buzzer_init(void);
void buzzer_start(void);
void buzzer_stop(void);
void Motor_init(void);
void Motor_cw(void);
void Motor_acw(void);
void Motor_stop(void);
void Motor_hold(void);


#endif /* MOTOR_BUZZER_H_ */
