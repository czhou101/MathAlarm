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
//END display control


//clear display
#define LCD_CLEAR_DISPLAY   0x01


//sets cursor move direction and specifies display shift
#define LCD_ENTRYMODE_SET   0x04    //Value below determines cursor move direction (decrement by default)
#define LCD_CURSOR_INCR     0x02
//END entry mode set

// set cursor position
#define LCD_RETURNHOME      0x02    //!< Set cursor position to zero
#define LCD_SETDDRAMADDR    0x80    //!< Used to set the DDRAM (Display Data RAM)

#define LCD_DISPLAYCONTROL  0x08    //!< Controls the display; does stuff like turning it off and on
#define LCD_CURSORON        0x02    //!< Turns the cursor on
#define LCD_CURSOROFF       0x00    //!< Turns the cursor off
#define LCD_BLINKON         0x01    //!< Turns on the blinking cursor
#define LCD_BLINKOFF        0x00    //!< Turns off the blinking cursor

void spi_send(const uint8_t data);

void init_spi(void);

void command_4bits(uint8_t data);

void command(uint8_t data);

void write_char(unsigned char ch);

void write_string(char str[]);

void init_LCD(void);

void clear_screen(void);

void home();

void setCursor(uint8_t, uint8_t);

void noCursor();

void cursor();

void noBlink();

void blink();

#endif /* ADAFRUIT_LCD_BACKPACK_INTERFACE_H_ */
