/*
 * File: LCD_Driver.c
 * Author: 	Paweł Sporna (pawel.sporna@gmail.com)
 * Date: 	12.10.2021
 *
 * Summary of File:
 *
 * 	This file contains drivers for LCD display 16x2 characters with soldered converter PCF8574.
 * 	Based on provided inputs in Init function, the drivers can by itself initialize display, display strings, floating and integer numbers,
 * 	display, set a cursor and clear the display space.
 * 	Displaying data is possible with formatting to the right side of designated space.
 */

#include "LCD_Driver.h"
#include <stdio.h>

void LCDI2C_Set_Cursor_Dir(LCDI2C_HandleTypeDef* lcd_handle, uint8_t dir)
{
	if(dir == LCDI2C_CURSOR_RIGHT || dir == LCDI2C_CURSOR_LEFT)
	{
		_LCDI2C_send_cmd(lcd_handle, dir);
	}
}

void LCDI2C_Set_Cursor(LCDI2C_HandleTypeDef* lcd_handle, uint8_t row, uint8_t col)
{
	// check if input is proper
	if(row < 2 && col < 16)
	{
		// update local Adress Counter
		lcd_handle->_AC = row * 0x40 + col;

		// send command to set cursor with updated local Adress Counter
		_LCDI2C_send_cmd(lcd_handle, 0x80 | lcd_handle->_AC);
	}
}

void LCDI2C_Clear_Disp(LCDI2C_HandleTypeDef* lcd_handle)
{
	// send the clear display command
	_LCDI2C_send_cmd(lcd_handle, LCDI2C_DISPLAY_CLEAR);
}

void _LCDI2C_send_char(LCDI2C_HandleTypeDef* lcd_handle, uint8_t ch)
{
	// check if local Adress Counter is set to out of row display
	if(lcd_handle->_AC == 0x10){

		// change local Adress Counter to next row
		lcd_handle->_AC = 0x40;

		// send command to set cursor with updated local Adress Counter
		_LCDI2C_send_cmd(lcd_handle, 0xC0);
	}

	// Check if local Adress Counter is out of display
	if(lcd_handle->_AC <= 0x4F){

		// Divide 8-bit ASCII character into two 4-bit nibbles
		uint8_t h_nibble = ch&0xF0;
		uint8_t l_nibble = (ch&0x0F) << 4;

		// Complete two 4-bit nibbles with 4 bit instructions: 0xD - RS,RW,E,BT, 0x9 - RS,RW,BT
		uint8_t arr[4] = {h_nibble | 0x0D, h_nibble | 0x09, l_nibble | 0x0D, l_nibble | 0x09};

		// Transmit data to display
		HAL_I2C_Master_Transmit(&(lcd_handle->i2c_handle), (lcd_handle->disp_addr)<<1, (uint8_t*)&arr, 4, HAL_MAX_DELAY);

		// Increment Adress Counter
		lcd_handle->_AC++;
	}

}

void LCDI2C_Send_Int(LCDI2C_HandleTypeDef* lcd_handle, uint32_t num)
{
	// declare char buffer to store the number
	char buff[16];

	// convert integer into string
	sprintf(buff, "%lu", num);

	// send char buffer to display
	LCDI2C_Send_String(lcd_handle, buff);
}

void LCDI2C_Send_Int_Spc(LCDI2C_HandleTypeDef* lcd_handle, uint32_t num, int8_t space)
{
	// declare char buffer to store the number
	char buff[16];

	// declare the variable to store NULL place
	uint8_t nullIndx = 0;

	// convert integer into string
	sprintf(buff, "%lu", num);

	// find where is NULL character
	while(buff[nullIndx] != '\0') {nullIndx++;}

	// if number of digits is higher than defined space display #
	if(nullIndx > space){
		for(int i = 0; i < space; i++){
			_LCDI2C_send_char(lcd_handle, '#');
		}
	}

	else{
		// format number to right side of defined space
		for(int i = 0; i < space-nullIndx; i++){
			_LCDI2C_send_char(lcd_handle,' ');
		}
		// send char buffer to display
		LCDI2C_Send_String(lcd_handle, buff);
	}
}

void LCDI2C_Send_Float(LCDI2C_HandleTypeDef* lcd_handle, float num, uint8_t prec)
{
	// maximum digits after dot is 9
	if(prec > 9) prec = 9;

	// declare buffer to store xf, where x is value of digits after dot
	char prec_buff[2];
	sprintf(prec_buff, "%i", prec);
	strcat(prec_buff, "f");

	// combine two buffers into %.xf, where x is value of digits after dot
	char dest_buff[4] = "%.";
	strcat(dest_buff, prec_buff);

	// convert float number into string using previously defined buffers
	char num_buff[16];
	sprintf(num_buff, dest_buff, num);

	// send char buffer to display
	LCDI2C_Send_String(lcd_handle, num_buff);
}

void LCDI2C_Send_Float_Spc(LCDI2C_HandleTypeDef* lcd_handle, float num, uint8_t prec, int8_t space)
{
	// maximum digits after dot is 9
	if(prec > 9) prec = 9;

	// declare buffer to store xf, where x is value of digits after dot
	char prec_buff[2];
	sprintf(prec_buff, "%i", prec);
	strcat(prec_buff, "f");

	// combine two buffers into %.xf, where x is value of digits after dot
	char dest_buff[4] = "%.";
	strcat(dest_buff, prec_buff);

	// convert float number into string using previously defined buffers
	char num_buff[16];
	sprintf(num_buff, dest_buff, num);

	// declare the variable to store NULL place
	uint8_t nullIndx = 0;

	// find where is NULL character
	while(num_buff[nullIndx] != '\0') {nullIndx++;}

	// if number of digits is higher than defined space display #
	if(nullIndx > space){
		for(int i = 0; i < space; i++){
			_LCDI2C_send_char(lcd_handle, '#');
		}
	}

	else{
		// format number to right side of defined space
		for(int i = 0; i < space-nullIndx; i++){
			_LCDI2C_send_char(lcd_handle,' ');
		}

		// send char buffer to display
		LCDI2C_Send_String(lcd_handle, num_buff);
	}
}

void LCDI2C_Send_String (LCDI2C_HandleTypeDef* lcd_handle, char *str)
{
	// send one by one each character until NULL character
	while (*str) _LCDI2C_send_char (lcd_handle, *str++);
}


void LCDI2C_Init(LCDI2C_HandleTypeDef* lcd_handle)
{
	// initial procedure to configure display in 4-bit mode
	HAL_Delay(50);
	_LCDI2C_send_cmd(lcd_handle, LCDI2C_8BIT);
	HAL_Delay(5);
	_LCDI2C_send_cmd(lcd_handle, LCDI2C_8BIT);
	HAL_Delay(1);
	_LCDI2C_send_cmd(lcd_handle, LCDI2C_8BIT);
	HAL_Delay(10);
	_LCDI2C_send_cmd(lcd_handle, lcd_handle->disp_bit_mode);
	HAL_Delay(10);

	// select 1 or 2 line display
	_LCDI2C_send_cmd(lcd_handle, lcd_handle->disp_line);
	HAL_Delay(2);
	_LCDI2C_send_cmd(lcd_handle, LCDI2C_DISPLAY_OFF);
	HAL_Delay(2);

	// clear the display
	_LCDI2C_send_cmd(lcd_handle, LCDI2C_DISPLAY_CLEAR);
	HAL_Delay(2);

	// select left or right cursor direction
	_LCDI2C_send_cmd(lcd_handle, lcd_handle->cursor_direction);
	HAL_Delay(2);

	// select visible or non visible and blinking or constant cursor
	_LCDI2C_send_cmd(lcd_handle, LCDI2C_DISPLAY_ON | lcd_handle->cursor | lcd_handle->cursor_blink);
}

void _LCDI2C_send_cmd(LCDI2C_HandleTypeDef *lcd_handle, uint8_t cmd) {

	// declare variable to store I2C transmission status (not used yet)
	HAL_StatusTypeDef stat;

	// divide 8-bit ASCII character into two 4-bit nibbles
	uint8_t h_nibble = cmd & 0xF0;
	uint8_t l_nibble = (cmd & 0x0F) << 4;


	// Complete two 4-bit nibbles with 4 bit instructions: 0x0C - RW,E,BT, 0x08 - RW,BT
	uint8_t arr[4] = { h_nibble | 0x0C, h_nibble | 0x08, l_nibble | 0x0C,
			l_nibble | 0x08 };

	// Transmit command to display
	stat = HAL_I2C_Master_Transmit(&(lcd_handle->i2c_handle), (lcd_handle->disp_addr) << 1,
			(uint8_t*) &arr, 4, HAL_MAX_DELAY);
}
