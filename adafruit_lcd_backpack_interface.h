/*
 * adafruit_lcd_backpack_interface.h
 *
 *  Created on: Apr 26, 2023
 *      Author: Chris Zhou
 */

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>


#ifndef ADAFRUIT_LCD_BACKPACK_INTERFACE_H_
#define ADAFRUIT_LCD_BACKPACK_INTERFACE_H_


#define ENABLE              0x20    //Mask to set ENABLE (E) bit high


//Sets interface data length
#define LCD_FUNCTION_SET    0x20    //Values below can be used to choose number of lines and font
#define LCD_2_LINES         0x08
#define LCD_5x10_FONT       0x04
//END function set


//Controls display
#define LCD_DISPLAY_CTRL    0x08    //Values below can be used to turn on display/cursor/cursor blink
#define LCD_DISPLAY_ON      0x04
#define LCD_CURSOR_ON       0x02
#define LCD_CURSOR_BLINK    0x01
//END display control


//clear display
#define LCD_CLEAR_DISPLAY   0x01


//sets cursor move direction and specifies display shift
#define LCD_ENTRYMODE_SET   0x04    //Value below determines cursor move direction (decrement by default)
#define LCD_CURSOR_INCR     0x02
//END entry mode set



void spi_send(const uint8_t data);

void init_spi(void);

void command_4bits(uint8_t data);

void command(uint8_t data);

void write_char(unsigned char ch);

void write_string(char str[]);

void init_LCD(void);


#endif /* ADAFRUIT_LCD_BACKPACK_INTERFACE_H_ */
