/*--------------------------------------------------------------------------------------------------------------------------
* module     : Fingerprint
* file name  : Fingerprint.h
* Author     : Abdelrahman OUN
* Created on : July 12, 2021
* description: Header file for Fingerprint R305 module
--------------------------------------------------------------------------------------------------------------------------*/

#ifndef Fingerprint_H
#define Fingerprint_H


/*******************************************************************************
*                        Defenations and Includes                              *
*******************************************************************************/


#include "usart.h"
#include "std_types.h"
#include "common_macros.h"

/*******************************************************************************
*                        Types Declaration                                    *
*******************************************************************************/

typedef uint8  FingPr_ReturnType;
#define MATCH            ((FingPr_ReturnType)0x01U)      /* Function Return Match */
#define NOT_MATCH        ((FingPr_ReturnType)0x00U)      /* Function Return NOT Match */



typedef struct {
	/*baud rate = N_baudRate * 9600 */
	uint8 N_baudRate;    

	/* security level form 1 to 5
	* 1 => FAR is the highest and FRR is the lowest
	* 5 => FAR is the lowest and FRR is the highest
	* */
	uint8 sec_level;

	/*
	* Data length the max length of the transferring data package
	* 0, 1, 2, 3, corresponding to 32 bytes, 64 bytes, 128 bytes, 256 bytes respectively
	*/
	uint8 data_length;
}FingerPrint_configType;


/*******************************************************************************
*                      Functions Prototypes                                   *
*******************************************************************************/
void Control();
void VfyPwd();
void FingPr_Init(FingerPrint_configType * Config_Ptr);
Std_ReturnType FingPr_AddFing(uint8 order);
Std_ReturnType FingPr_saveFinger(uint8 fing_num);
void FingPr_RemoveFing(uint8 fing_num);
void FingPr_RemoveAllFing(void);
FingPr_ReturnType FingPr_FingMatch(void);


#endif /* Fingerprint_H */
