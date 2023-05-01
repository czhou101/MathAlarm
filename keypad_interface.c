/*
 * keypad_interface.c
 *
 *  Created on: Apr 30, 2023
 *      Author: Chris Zhou
 */

#include <msp430.h>
#include <keypad_interface.h>

char keypad[4][3] = {{'1', '2', '3'},               //layout of 3x4 matrix keypad
                     {'4', '5', '6'},
                     {'7', '8', '9'},
                     {'*', '0', '#'}};

int keyPressed[4][3] = {{0, 0, 0},
                        {0, 0, 0},
                        {0, 0, 0},
                        {0, 0, 0}};


/**
 * Initializes GPIO pins 2.0 through 2.6
 */
void init_keypad(void) {
    //P1    P2    P3    P4    P5    P6    P7
    //C2    R1    C1    R4    C3    R3    R2
    //2.1   2.3   2.0   2.6   2.2   2.5   2.4

    P2SEL &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6); //set 2.0 thru 2.6 to GPIO
    P2SEL2 &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6);

    P2DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6; //set 2.0 thru 2.6 to output
    P2OUT |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6; //set 2.0 thru 2.6 to HIGH

}


/**
 * Checks if a button on the keypad was pressed
 * Returns:
 *  - The character that was pressed, if there was one, '\0' if no button was pressed
 */
char check_keypress() {
    int i, j;
    char read = '\0';
    for(i = 6; i >= 3; i--) {                   // loop through rows
        P2OUT ^= 1 << i;                        // set one row to 0
        for(j = 2; j >= 0; j--) {               // for each row, loop through all cols
            P2DIR ^= 1 << j;                    // set col to input
            if(!(P2IN & (1 << j))){             // if col input value is 0, button was pressed
                if(!keyPressed[i - 3][j]) {
                    read = keypad[i - 3][j];
                    keyPressed[i - 3][j] = 1;   // key down (for debouncing)
                }
            }
            else {
                keyPressed[i - 3][j] = 0;
            }
            P2DIR ^= 1 << j;
        }
        P2OUT ^= 1 << i;
    }

    return read;
}
