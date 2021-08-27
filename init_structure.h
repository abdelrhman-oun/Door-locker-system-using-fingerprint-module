#ifndef INIT_STRUCTURE_H
#define INIT_STRUCTURE_H

#include "Fingerprint.h"
#include "usart.h"
#include "timer.h"

Usart_ConfigType uart_configstruct = {
	NO_PARITY,
	9600,
	SINGLE,
	EIGHT_BITS,
};

FingerPrint_configType fingpr_configstruct = {
	1,
	3,
	0
};

Timer_configType timer_config = {
	timer1,
	m_ctc,
	F_CPU_1024,
	i_ctc,
	0,
	2930
};//Timer configuration structure










#endif /* INIT_STRUCTURE_H */
