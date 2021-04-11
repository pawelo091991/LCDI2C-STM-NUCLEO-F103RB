/*
 * File: LCD_Driver.h
 * Author: 	Paweł Sporna (pawel.sporna@gmail.com)
 * Date: 	12.10.2021
 *
 * Summary of File:
 *
 * 	This file contains drivers for LCD display 16x2 characters with soldered converter PCF8574 of I2C bus.
 * 	Based on provided inputs in Init function, the drivers can by itself initialize display, display strings, floating and integer numbers,
 * 	display, set a cursor and clear the display space
 * 	Displaying data is possible with formatting to the right side of designated space
 */

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#define LCDI2C_8BIT 				0x30
#define LCDI2C_4BIT 				0x20
#define LCDI2C_2LINE 				0x28
#define LCDI2C_1LINE				0x20
#define LCDI2C_CURSOR_ON			0x0A
#define LCDI2C_CURSOR_OFF			0x08
#define LCDI2C_CURSOR_BLINK_ON		0x09
#define LCDI2C_CURSOR_BLINK_OFF		0x08
#define LCDI2C_DISPLAY_ON			0x0C
#define LCDI2C_DISPLAY_OFF			0x08
#define LCDI2C_DISPLAY_CLEAR		0x01
#define LCDI2C_CURSOR_RIGHT			0x06
#define LCDI2C_CURSOR_LEFT			0x04

#define LCDI2C_DEV_ADRESS			0x3F

#define LEFT 0x00
#define RIGHT 0x01

#include "stm32f1xx.h"
#include <string.h>

typedef struct LCDI2C_HandleTypeDef{
	I2C_HandleTypeDef i2c_handle;
	uint8_t disp_addr;	//converter I2C adress
	uint8_t disp_bit_mode;	//8bit, 4 bit
	uint8_t disp_line;	//2 line, 1 line
	uint8_t cursor;		//on, off
	uint8_t cursor_blink;	//blink on, blink off
	uint8_t cursor_direction;	//going to right, going to left

	uint8_t _AC;
}LCDI2C_HandleTypeDef;


/*
 * void _LCDI2C_send_cmd(LCDI2C_HandleTypeDef* lcd_handle, uint8_t cmd)
 *
 * Summary of the _LCDI2C_send_cmd function:
 * 	The _LCDI2C_send_cmd function sends 8-bit command to display
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 * 				cmd: 8bit command to display
 *
 * Return Value : None
 *
 * Description:
 * 	This function divide 8-bit command into two 4 bit nibbles and send it via I2C to display
 */
void _LCDI2C_send_cmd(LCDI2C_HandleTypeDef* lcd_handle, uint8_t cmd);


/*
 * void _LCDI2C_send_char(LCDI2C_HandleTypeDef* lcd_handle, uint8_t ch);
 *
 * Summary of the _LCDI2C_send_char function:
 * 	The _LCDI2C_send_char function sends an ASCII character to display
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 * 				ch: an ASCII character to be displayed
 *
 * Return Value : None
 *
 * Description:
 * 	This function divide 8-bit ASCII character into two 4 bit nibbles and sends it via I2C to display
 */
void _LCDI2C_send_char(LCDI2C_HandleTypeDef* lcd_handle, uint8_t ch);


/*
 * void LCDI2C_Clear_Disp(LCDI2C_HandleTypeDef* lcd_handle);
 *
 * Summary of the LCDI2C_Clear_Disp function:
 * 	The LCDI2C_Clear_Disp function clears completely the display from any character/number
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 *
 * Return Value : None
 *
 * Description:
 * 	This function sends 8-bit clear command to the display via I2C bus
 */
void LCDI2C_Clear_Disp(LCDI2C_HandleTypeDef* lcd_handle);


/*
 * void LCDI2C_Set_Cursor_Dir(LCDI2C_HandleTypeDef* lcd_handle, uint8_t dir)
 *
 * Summary of the LCDI2C_Set_Cursor_Dir function:
 * 	The LCDI2C_Set_Cursor_Dir function sets cursor direction to left or right
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 * 				dir: cursor direction LCDI2C_CURSOR_RIGHT or LCDI2C_CURSOR_LEFT
 *
 * Return Value : None
 *
 * Description:
 * 	This function sends 8-bit clear command to the display via I2C bus to set cursor direction
 * 	If dir parameter is not LCDI2C_CURSOR_RIGHT or LCDI2C_CURSOR_LEFT, it is ignored
 */
void LCDI2C_Set_Cursor_Dir(LCDI2C_HandleTypeDef* lcd_handle, uint8_t dir);


/*
 * void LCDI2C_Set_Cursor(LCDI2C_HandleTypeDef* lcd_handle, uint8_t row, uint8_t col)
 *
 * Summary of the LCDI2C_Set_Cursor function:
 * 	The LCDI2C_Set_Cursor_Dir function places cursor in row and column defined by parameters
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 * 				row: defines row in which cursor shall be placed: 0 or 1
 * 				col: defines column in which cursor shall be placed: 0 to 15
 *
 * Return Value : None
 *
 * Description:
 * 	This function sends 8-bit set command to the display via I2C bus to set cursor placement
 * 	It calculates Adress Counter in Display based on human friendly row and column parameters
 * 	If row and col parameters are outside of defined range, they are ignored
 */
void LCDI2C_Set_Cursor(LCDI2C_HandleTypeDef* lcd_handle, uint8_t row, uint8_t col);


/*
 * void LCDI2C_Send_String(LCDI2C_HandleTypeDef* lcd_handle, char *str)
 *
 * Summary of the LCDI2C_Send_String function:
 * 	The LCDI2C_Send_String function sends one by one character pointed by str
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 * 				str: a pointer to character string to be displayed
 *
 * Return Value : None
 *
 * Description:
 * 	This function sends one by one 8-bit character data to the display via I2C bus to display it
 * 	It ends when str point to NULL character
 */
void LCDI2C_Send_String(LCDI2C_HandleTypeDef* lcd_handle, char *str);


/*
 * void LCDI2C_Send_Int(LCDI2C_HandleTypeDef* lcd_handle, uint32_t num)
 *
 * Summary of the LCDI2C_Send_Int function:
 * 	The LCDI2C_Send_Int function display 32 bit unsigned integer number
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 * 				num: 32-bit unsigned integer number to be displayed
 *
 * Return Value : None
 *
 * Description:
 * 	This function converts 32-bit unsigned integer into 16 char array and one by
 * 	one send data to the display via I2C bus to display it
 */
void LCDI2C_Send_Int(LCDI2C_HandleTypeDef* lcd_handle, uint32_t num);


/*
 * void LCDI2C_Send_Int_Spc(LCDI2C_HandleTypeDef* lcd_handle, uint32_t num, int8_t space)
 *
 * Summary of the LCDI2C_Send_Int_Spc function:
 * 	The LCDI2C_Send_Int_Spc function sends 32 bit unsigned integer number
 * 	and formatting to the right side of designated space
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 * 				num: 32-bit unsigned integer number to be displayed
 * 				space: space designated to display number aligned to right side
 *
 * Return Value : None
 *
 * Description:
 *
 * 	This function converts 32-bit unsigned integer into 16 char array and call LCDI2C_Send_String function
 * 	to display it into defined empty space with aligned it to the right side of this space
 * 	If number of characters to be displayed is larger that space, it will display "#" characters
 */
void LCDI2C_Send_Int_Spc(LCDI2C_HandleTypeDef* lcd_handle, uint32_t num, int8_t space);


/*
 * void LCDI2C_Send_Float(LCDI2C_HandleTypeDef* lcd_handle, float num, uint8_t prec)
 *
 * Summary of the LCDI2C_Send_Float function:
 * 	The LCDI2C_Send_Float function sends float number to display with defined precision
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 * 				num: float number to be displayed
 * 				prec: number of digits after dot: 0 to 9
 *
 * Return Value : None
 *
 * Description:
 * 	This function converts float into 16 char array with defined digits after dot, covnvert it
 * 	into char array and call LCDI2C_Send_String function
 *
 */
void LCDI2C_Send_Float(LCDI2C_HandleTypeDef* lcd_handle, float num, uint8_t prec);


/*
 * void LCDI2C_Send_Float_Spc(LCDI2C_HandleTypeDef* lcd_handle, float num, uint8_t prec, int8_t space)
 *
 * Summary of the LCDI2C_Send_Float_Spc function:
 * 	The LCDI2C_Send_Float_Spc function sends float number to display with defined precision
 * 	and formatting to the right side of designated space
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 * 				num: float number to be displayed
 * 				prec: number of digits after dot: 0 to 9
 * 				space: space designated to display number aligned to right side
 *
 * Return Value : None
 *
 * Description:
 * 	This function converts float into 16 char array with defined digits after dot, covnvert it
 * 	into char array and call LCDI2C_Send_String function to display it into defined empty space
 * 	with aligned it to the right side of this space
 */
void LCDI2C_Send_Float_Spc(LCDI2C_HandleTypeDef* lcd_handle, float num, uint8_t prec, int8_t space);

/*
 * void LCDI2C_Init(LCDI2C_HandleTypeDef* lcd_handle)
 *
 * Summary of the LCDI2C_Init function:
 * 	The LCDI2C_Init function initiate display
 *
 * Parameters: 	lcd_handle: a pointer to object with LCD and I2C configuration data
 *
 * Return Value : None
 *
 * Description:
 * 	The LCDI2C_Init function initatate display based on data provided by struct pointed by lcd_handle
 */
void LCDI2C_Init(LCDI2C_HandleTypeDef* lcd_handle);


#endif /* LCD_DRIVER_H_ */
