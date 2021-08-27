/*
 * EEPROM.h
 *
 * Created: 7/11/2021 8:46:35 PM
 *  Author: Ahmed Adel
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_


#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"





void EEPROM_writeByte(uint16 AddrU16, uint8 DataU8);
uint8 EEPROM_readByte(uint16 AddrU16);

#endif /* EEPROM_H_ */
