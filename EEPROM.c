/*
 * GccApplication1.c
 *
 * Created: 7/11/2021 8:44:15 PM
 * Author : Ahemd Adel
 */ 

#include "EEPROM.h"

void EEPROM_writeByte(uint16 AddrU16, uint8 DataU8)
{
	while (BIT_IS_SET(EECR,EEWE));           //Wait for last write to finish
	EEAR = AddrU16;                          //Set the EEPRO Address
	EEDR = DataU8;                           //Set the EEPROM Data
	SET_BIT(EECR,EEMWE);                     //Set the master write enable bit in control register so for 
	SET_BIT(EECR,EEWE);
}


uint8 EEPROM_readByte(uint16 AddrU16)
{
	while (BIT_IS_SET(EECR,EEWE));
	EEAR = AddrU16;
	SET_BIT(EECR,EERE);
	return EEDR;
}