#include <msp430.h> 
#include <adafruit_lcd_backpack_interface.h>
#include <keypad_interface.h>




/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	init_spi();

    init_LCD();

    init_keypad();

    while(1) {                                      //We can maybe use interrupts here to schedule a button check so we don't have to check every cycle? idk
        char pressedBtn = check_keypress();
        if(pressedBtn != '\0') {
            write_char(pressedBtn);
        }
    }



	return 0;
}
