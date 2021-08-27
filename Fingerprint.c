/*--------------------------------------------------------------------------------------------------------------------------
* module     : Fingerprint
* file name  : Fingerprint.c
* Author     : Abdelrahman OUN
* Created on : July 12, 2021
* description: source file for Fingerprint R305 module
--------------------------------------------------------------------------------------------------------------------------*/


/*******************************************************************************
*                        Definations and Includes                              *
*******************************************************************************/

#include"Fingerprint.h"

#define SUCESS					0X00


/*******************************************************************************
*                      Functions Prototypes                                   *
*******************************************************************************/

Std_ReturnType SetSysPara(uint8 Para_num, uint8 Para_content);
Std_ReturnType GenImg(void);
Std_ReturnType Img2Tz(uint8 buffer_id);
Std_ReturnType RegModel(void);
Std_ReturnType Store(uint8 finger_number);
Std_ReturnType DeletChar(uint8 num_segment, uint8 page_id);
Std_ReturnType Empty(void);
Std_ReturnType Search(uint8 buffer_id, uint8 start_page, uint8 num_page);


/*******************************************************************************
*                      Functions Defination                                    *
*******************************************************************************/


void FingPr_Init(FingerPrint_configType * Config_Ptr) {

	Std_ReturnType result;

	/* Set module system’s basic parameters */
	/* Set module system Baud rate (Parameter Number: 04h) */
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = SetSysPara(0x04, Config_Ptr->N_baudRate);
	//}
	/* Set module system Security Level (Parameter Number: 05h) */
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = SetSysPara(0x05, Config_Ptr->sec_level);
	//}

	/* Set module system Data package length (Parameter Number: 06h) */
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = SetSysPara(0x06, Config_Ptr->data_length);
	//}
}

Std_ReturnType FingPr_AddFing(uint8 order) {

	Std_ReturnType result;

	/* detect finger and store the detected finger image in ImageBuffer */
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = GenImg();
	//}

	/* generate character file from the original finger image in ImageBuffer and store the file in CharBuffer 1 or 2*/
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = Img2Tz(order);
	//}
	return result;
}

Std_ReturnType FingPr_saveFinger(uint8 fing_num) {

	Std_ReturnType result;

	/*combine information of character files from CharBuffer1 and CharBuffer2 and generate a template*/
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = RegModel();
	//}

	/*store the template of specified buffer (Buffer1/Buffer2) at the designated location of Flash library*/
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = Store(fing_num);
	//}
	return result;
}

void FingPr_RemoveFing(uint8 fing_num) {

	Std_ReturnType result;

	/*delete a segment 1 of templates of Flash library started from the specified location (fing_num)*/
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = DeletChar(0x01, fing_num);
	//}
}

void FingPr_RemoveAllFing(void) {

	Std_ReturnType result;

	/*delete all the templates in the Flash library*/
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = Empty();
	//}
}


FingPr_ReturnType FingPr_FingMatch(void) {

	Std_ReturnType result;

	/* detect finger and store the detected finger image in ImageBuffer */
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = GenImg();
	//}

	/* generate character file from the original finger image in ImageBuffer and store the file in CharBuffer1*/
	result = E_NOT_OK;
	//while (result == E_NOT_OK) {
		result = Img2Tz(0x01);
	//}

	/* search the whole finger library for the template that matches the one in CharBuffer1 When found, PageID will be returned*/
	/* assumption : max number of finger saved 9 */
	result = E_NOT_OK;
	result = Search(0x01, 0x01, 0x09);
	
	if (result == E_NOT_OK) {
		return NOT_MATCH;
	}
	else {
		return MATCH;
	}
}


/*******************************************************************************
*                     Local Functions Defination                               *
*******************************************************************************/

/* Set module system’s basic parameters */

Std_ReturnType SetSysPara(uint8 Para_num, uint8 Para_content) {

	uint8 i = 0;
	uint16 sum = 0x00;
	uint8 temp;

	/* send header 2 bytes*/
	USART_sendByte(0xEF);
	USART_sendByte(0x01);
	/* send module address 4 bytes */
	
	(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	/* send Package identifier 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* send Package length 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(0x05);
	sum = sum + 0x05;
	/* send package content */
	/* send Instruction code 1 byte */
	USART_sendByte(0x0E);
	sum = sum + 0x0E;
	/* sned Parameter number 1 byte */
	USART_sendByte(Para_num);
	sum = sum + Para_num;
	/* sned Parameter content 1 byte */
	USART_sendByte(Para_content);
	sum = sum + Para_content;
	/* sned check sum 2 byte */
	USART_sendByte((sum >> 8));
	USART_sendByte((uint8)sum);

	/* receive Acknowledge package */
	/* receive header 2 bytes*/
	/* then receive moduel address 4 bytes*/
	for (i = 0; i < 6; i++) {
		temp = USART_receiveByte();
	}
	/* receive Package identifier 1 byte */
	temp = USART_receiveByte();
	//if (temp != 0x07) {
		//return E_NOT_OK;
	//}
	/* receive Package length 2 byte */
	temp = USART_receiveByte();
	temp = USART_receiveByte();
	//if (temp != 0x03) {
		//return E_NOT_OK;
	//}
	/* receive confirmation code 1 byte*/
	temp = USART_receiveByte();
	//if (temp != SUCESS) {
		//return E_NOT_OK;
	//}
	/*recive sum 2 bytes*/
	temp = USART_receiveByte();
	temp = USART_receiveByte();

	return E_OK;

}


/* detecting finger and store the detected finger image in ImageBuffer while returning successfull confirmation code;
* If there is no finger, returned confirmation code would be “can’t detect finger”*/

Std_ReturnType GenImg(void) {

	uint8 i = 0;
	uint16 sum = 0x00;
	uint8 temp;

	/* send header 2 bytes*/
	USART_sendByte(0xEF);
	USART_sendByte(0x01);
	/* send module address 4 bytes */
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	/* send Package identifier 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* send Package length 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(0x03);
	sum = sum + 0x03;
	/* send package content */
	/* send Instruction code 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* sned check sum 2 byte */
	USART_sendByte((sum >> 8));
	USART_sendByte((uint8)sum);

	/* receive Acknowledge package */
	/* receive header 2 bytes*/
	/* then receive moduel address 4 bytes*/
	for (i = 0; i < 6; i++) {
		temp = USART_receiveByte();
	}
	/* receive Package identifier 1 byte */
	temp = USART_receiveByte();
	/*if (temp != 0x07) {
		return E_NOT_OK;
	}
	/* receive Package length 2 byte */
	temp = USART_receiveByte();
	temp = USART_receiveByte();
	/*if (temp != 0x03) {
		return E_NOT_OK;
	}
	/* receive confirmation code 1 byte*/
	temp = USART_receiveByte();
	/*if (temp != SUCESS) {
		return E_NOT_OK;
	}
	/*recive sum 2 bytes*/
	temp = USART_receiveByte();
	temp = USART_receiveByte();

	return E_OK;


}


/* generate character file from the original finger image in ImageBuffer and store
*  the file in CharBuffer1 or CharBuffer2 */

Std_ReturnType Img2Tz(uint8 buffer_id) {
	uint8 i = 0;
	uint16 sum = 0x00;
	uint8 temp;

	/* send header 2 bytes*/
	USART_sendByte(0xEF);
	USART_sendByte(0x01);
	/* send module address 4 bytes */
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	/* send Package identifier 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* send Package length 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(0x04);
	sum = sum + 0x04;
	/* send package content */
	/* send Instruction code 1 byte */
	USART_sendByte(0x02);
	sum = sum + 0x02;
	/* sned buffer number 1 byte */
	USART_sendByte(buffer_id);
	sum = sum + buffer_id;
	/* sned check sum 2 byte */
	USART_sendByte((sum >> 8));
	USART_sendByte((uint8)sum);

	/* receive Acknowledge package */
	/* receive header 2 bytes*/
	/* then receive moduel address 4 bytes*/
	for (i = 0; i < 6; i++) {
		temp = USART_receiveByte();
	}
	/* receive Package identifier 1 byte */
	temp = USART_receiveByte();
	/*if (temp != 0x07) {
		return E_NOT_OK;
	}
	/* receive Package length 2 byte */
	temp = USART_receiveByte();
	temp = USART_receiveByte();
	/*if (temp != 0x03) {
		return E_NOT_OK;
	}
	/* receive confirmation code 1 byte*/
	temp = USART_receiveByte();
	/*if (temp != SUCESS) {
		return E_NOT_OK;
	}
	/*recive sum 2 bytes*/
	temp = USART_receiveByte();
	temp = USART_receiveByte();

	return E_OK;

}


/* combine information of character files from CharBuffer1 and CharBuffer2 and
* generate a template which is stroed back in both CharBuffer1 and CharBuffer2 */

Std_ReturnType RegModel(void) {

	uint8 i = 0;
	uint16 sum = 0x00;
	uint8 temp;

	/* send header 2 bytes*/
	USART_sendByte(0xEF);
	USART_sendByte(0x01);
	/* send module address 4 bytes */
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	/* send Package identifier 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* send Package length 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(0x03);
	sum = sum + 0x03;
	/* send package content */
	/* send Instruction code 1 byte */
	USART_sendByte(0x05);
	sum = sum + 0x05;
	/* sned check sum 2 byte */
	USART_sendByte((sum >> 8));
	USART_sendByte((uint8)sum);

	/* receive Acknowledge package */
	/* receive header 2 bytes*/
	/* then receive moduel address 4 bytes*/
	for (i = 0; i < 6; i++) {
		temp = USART_receiveByte();
	}
	/* receive Package identifier 1 byte */
	temp = USART_receiveByte();
	/*if (temp != 0x07) {
		return E_NOT_OK;
	}
	/* receive Package length 2 byte */
	temp = USART_receiveByte();
	temp = USART_receiveByte();
	/*if (temp != 0x03) {
		return E_NOT_OK;
	}
	/* receive confirmation code 1 byte*/
	temp = USART_receiveByte();
	/*if (temp != SUCESS) {
		return E_NOT_OK;
	}
	/*recive sum 2 bytes*/
	temp = USART_receiveByte();
	temp = USART_receiveByte();

	return E_OK;

}


/*store the template of specified buffer (Buffer1/Buffer2) at the designated location of Flash library*/

Std_ReturnType Store(uint8 finger_number) {

	uint8 i = 0;
	uint16 sum = 0x00;
	uint8 temp;

	/* send header 2 bytes*/
	USART_sendByte(0xEF);
	USART_sendByte(0x01);
	/* send module address 4 bytes */
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	/* send Package identifier 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* send Package length 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(0x06);
	sum = sum + 0x06;
	/* send package content */
	/* send Instruction code 1 byte */
	USART_sendByte(0x06);
	sum = sum + 0x06;
	/* sned buffer number 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* sned location number 2 byte */
	USART_sendByte(0x00);
	USART_sendByte(finger_number);
	sum = sum + finger_number;
	/* sned check sum 2 byte */
	USART_sendByte((sum >> 8));
	USART_sendByte((uint8)sum);

	/* receive Acknowledge package */
	/* receive header 2 bytes*/
	/* then receive moduel address 4 bytes*/
	for (i = 0; i < 6; i++) {
		temp = USART_receiveByte();
	}
	/* receive Package identifier 1 byte */
	temp = USART_receiveByte();
	/*if (temp != 0x07) {
		return E_NOT_OK;
	}
	/* receive Package length 2 byte */
	temp = USART_receiveByte();
	temp = USART_receiveByte();
	/*if (temp != 0x03) {
		return E_NOT_OK;
	}
	/* receive confirmation code 1 byte*/
	temp = USART_receiveByte();
	/*if (temp != SUCESS) {
		return E_NOT_OK;
	}
	/*recive sum 2 bytes*/
	temp = USART_receiveByte();
	temp = USART_receiveByte();

	return E_OK;
}


/* delete a segment (N) of templates of Flash library started from the specified location (or PageID) */

Std_ReturnType DeletChar(uint8 num_segment, uint8 page_id) {

	uint8 i = 0;
	uint16 sum = 0x00;
	uint8 temp;

	/* send header 2 bytes*/
	USART_sendByte(0xEF);
	USART_sendByte(0x01);
	/* send module address 4 bytes */
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	/* send Package identifier 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* send Package length 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(0x07);
	sum = sum + 0x07;
	/* send package content */
	/* send Instruction code 1 byte */
	USART_sendByte(0x0C);
	sum = sum + 0x0C;
	/* sned page number 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(page_id);
	sum = sum + page_id;
	/* sned number of templates to be deleted 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(num_segment);
	sum = sum + num_segment;
	/* sned check sum 2 byte */
	USART_sendByte((sum >> 8));
	USART_sendByte((uint8)sum);

	/* receive Acknowledge package */
	/* receive header 2 bytes*/
	/* then receive moduel address 4 bytes*/
	for (i = 0; i < 6; i++) {
		temp = USART_receiveByte();
	}
	/* receive Package identifier 1 byte */
	temp = USART_receiveByte();
	/*if (temp != 0x07) {
		return E_NOT_OK;
	}
	/* receive Package length 2 byte */
	temp = USART_receiveByte();
	temp = USART_receiveByte();
	/*if (temp != 0x03) {
		return E_NOT_OK;
	}
	/* receive confirmation code 1 byte*/
	temp = USART_receiveByte();
	/*if (temp != SUCESS) {
		return E_NOT_OK;
	}
	/*recive sum 2 bytes*/
	temp = USART_receiveByte();
	temp = USART_receiveByte();

	return E_OK;
}


/* delete all the templates in the Flash library */

Std_ReturnType Empty(void) {

	uint8 i = 0;
	uint16 sum = 0x00;
	uint8 temp;

	/* send header 2 bytes*/
	USART_sendByte(0xEF);
	USART_sendByte(0x01);
	/* send module address 4 bytes */
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	/* send Package identifier 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* send Package length 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(0x03);
	sum = sum + 0x03;
	/* send package content */
	/* send Instruction code 1 byte */
	USART_sendByte(0x0D);
	sum = sum + 0x0D;
	/* sned check sum 2 byte */
	USART_sendByte((sum >> 8));
	USART_sendByte((uint8)sum);

	/* receive Acknowledge package */
	/* receive header 2 bytes*/
	/* then receive moduel address 4 bytes*/
	for (i = 0; i < 6; i++) {
		temp = USART_receiveByte();
	}
	/* receive Package identifier 1 byte */
	temp = USART_receiveByte();
	/*if (temp != 0x07) {
		return E_NOT_OK;
	}
	/* receive Package length 2 byte */
	temp = USART_receiveByte();
	temp = USART_receiveByte();
	/*if (temp != 0x03) {
		return E_NOT_OK;
	}
	/* receive confirmation code 1 byte*/
	temp = USART_receiveByte();
	/*if (temp != SUCESS) {
		return E_NOT_OK;
	}
	/*recive sum 2 bytes*/
	temp = USART_receiveByte();
	temp = USART_receiveByte();

	return E_OK;

}

/* search the whole finger library for the template that matches the one in
* CharBuffer1 or CharBuffer2. When found, PageID will be returned */

Std_ReturnType Search(uint8 buffer_id, uint8 start_page, uint8 num_page) {

	uint8 i = 0;
	uint16 sum = 0x00;
	uint8 temp;

	/* store matched page */
	uint16 mat_page;
	/* store matche score */
	uint16 mat_score;

	/* send header 2 bytes*/
	USART_sendByte(0xEF);
	USART_sendByte(0x01);
	/* send module address 4 bytes */
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	/* send Package identifier 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* send Package length 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(0x08);
	sum = sum + 0x08;
	/* send package content */
	/* send Instruction code 1 byte */
	USART_sendByte(0x04);
	sum = sum + 0x04;
	/* sned buffer ID 1 byte */
	USART_sendByte(buffer_id);
	sum = sum + buffer_id;
	/* sned Parameter Start Page 2 byte */
	USART_sendByte(0x00);
	USART_sendByte(start_page);
	sum = sum + start_page;
	/* sned Parameter number of pages 2 byte */
	USART_sendByte(0x00);
	USART_sendByte(num_page);
	sum = sum + num_page;
	/* sned check sum 2 byte */
	USART_sendByte((sum >> 8));
	USART_sendByte((uint8)sum);

	/* receive Acknowledge package */
	/* receive header 2 bytes*/
	/* then receive moduel address 4 bytes*/
	for (i = 0; i < 6; i++) {
		temp = USART_receiveByte();
	}
	/* receive Package identifier 1 byte */
	temp = USART_receiveByte();
	/*if (temp != 0x07) {
		return E_NOT_OK;
	}
	/* receive Package length 2 byte */
	temp = USART_receiveByte();
	temp = USART_receiveByte();
	/*if (temp != 0x07) {
		return E_NOT_OK;
	}
	/* receive confirmation code 1 byte*/
	temp = USART_receiveByte();
	/*if (temp != SUCESS) {
		return E_NOT_OK;
	}

	/* receive matched page ID 2 byte */
	temp = USART_receiveByte();
	mat_page = ((uint16)temp) << 8;
	temp = USART_receiveByte();
	mat_page = (mat_page & 0xFF00) | ((uint16)temp & 0x00FF);

	/* receive Match Score 2 byte */
	temp = USART_receiveByte();
	mat_score = ((uint16)temp) << 8;
	temp = USART_receiveByte();
	mat_score = (mat_score & 0xFF00) | ((uint16)temp & 0x00FF);

	/*recive sum 2 bytes*/
	temp = USART_receiveByte();
	temp = USART_receiveByte();

	return E_OK;

}


void VfyPwd(){
	
	uint8 i = 0;
	uint16 sum = 0x00;
	uint8 temp;

	/* send header 2 bytes*/
	USART_sendByte(0xEF);
	USART_sendByte(0x01);
	/* send module address 4 bytes */
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	USART_sendByte(0xFF);
	/* send Package identifier 1 byte */
	USART_sendByte(0x01);
	sum = sum + 0x01;
	/* send Package length 2 bytes */
	USART_sendByte(0x00);
	USART_sendByte(0x07);
	sum = sum + 0x07;
	/* send package content */
	/* send Instruction code 1 byte */
	USART_sendByte(0x13);
	sum = sum + 0x13;
	/* sned password 4 byte */
	USART_sendByte(0xFF);
	sum = sum + 0xFF;
	USART_sendByte(0xFF);
	sum = sum + 0xFF;
	USART_sendByte(0xFF);
	sum = sum + 0xFF;
	USART_sendByte(0xFF);
	sum = sum + 0xFF;
	/* sned check sum 2 byte */
	USART_sendByte((sum >> 8));
	USART_sendByte((uint8)sum);
	
LCD_clearScreen();
LCD_displayString("IAM HERW");

	/* receive Acknowledge package */
	/* receive header 2 bytes*/
	/* then receive module address 4 bytes*/
	temp = USART_receiveByte();
	LCD_clearScreen();
	LCD_displayString("I5555");
	
	for (i = 0; i < 5; i++) {
		temp = USART_receiveByte();
	}
	/* receive Package identifier 1 byte */
	temp = USART_receiveByte();
	//if (temp != 0x07) {
	//return E_NOT_OK;
	//}
	/* receive Package length 2 byte */
	temp = USART_receiveByte();
	temp = USART_receiveByte();
	//if (temp != 0x03) {
	//return E_NOT_OK;
	//}
	/* receive confirmation code 1 byte*/
	temp = USART_receiveByte();
	//if (temp != SUCESS) {
	//return E_NOT_OK;
	//}
	/*receive sum 2 bytes*/
	temp = USART_receiveByte();
	temp = USART_receiveByte();

	return E_OK;

}

void Control(){
		uint8 i = 0;
		uint16 sum = 0x00;
		uint8 temp;

		/* send header 2 bytes*/
		USART_sendByte(0xEF);
		USART_sendByte(0x01);
		/* send module address 4 bytes */
		USART_sendByte(0xFF);
		USART_sendByte(0xFF);
		USART_sendByte(0xFF);
		USART_sendByte(0xFF);
		/* send Package identifier 1 byte */
		USART_sendByte(0x01);
		sum = sum + 0x01;
		/* send Package length 2 bytes */
		USART_sendByte(0x00);
		USART_sendByte(0x04);
		sum = sum + 0x04;
		/* send package content */
		/* send Instruction code 1 byte */
		USART_sendByte(0x17);
		sum = sum + 0x17;
		/* sned Control code 1 byte */
		USART_sendByte(0x01);
		sum = sum + 0x01;
		
		/* sned check sum 2 byte */
		USART_sendByte((sum >> 8));
		USART_sendByte((uint8)sum);
		
		LCD_clearScreen();
		LCD_displayString("IAM HERW");

		/* receive Acknowledge package */
		/* receive header 2 bytes*/
		/* then receive moduel address 4 bytes*/
		temp = USART_receiveByte();
		LCD_clearScreen();
		LCD_displayString("I5555");
		
		for (i = 0; i < 5; i++) {
			temp = USART_receiveByte();
		}
		/* receive Package identifier 1 byte */
		temp = USART_receiveByte();
		//if (temp != 0x07) {
		//return E_NOT_OK;
		//}
		/* receive Package length 2 byte */
		temp = USART_receiveByte();
		temp = USART_receiveByte();
		//if (temp != 0x03) {
		//return E_NOT_OK;
		//}
		/* receive confirmation code 1 byte*/
		temp = USART_receiveByte();
		//if (temp != SUCESS) {
		//return E_NOT_OK;
		//}
		/*recive sum 2 bytes*/
		temp = USART_receiveByte();
		temp = USART_receiveByte();

		return E_OK;

}