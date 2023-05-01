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
char check_keypress(void) {
    //check for button press
    //char btnVal = '\0';
    int i, j;
    for(i = 6; i >= 3; i--) {                       //For each row of the 4 rows,
        P2OUT ^= 1 << i;                            //set that row LOW
        for(j = 2; j >= 0; j--) {                   //Then loop through each col of the 3 cols
            P2DIR ^= 1 << j;                        //set that col to input
            if(!(P2IN & (1 << j))){                 //Then if the value of the col input is 0 --> col and row shorted --> button at (row, col) was pressed
                if(!keyPressed[i - 3][j]) {         //Check if the button went up before being pressed
                    //write_char(keypad[i - 3][j]);
                    keyPressed[i - 3][j] = 1;       //Mark that the button was pressed, so another character is not returned for the same button
                    //btnVal =  keypad[i - 3][j];
                    return keypad[i - 3][j];        //If yes, then return the character pressed,
                }                                   //press without the button first going up
            }                                       //If the button didn't go up before being pressed, then we know a character was already returned for this button press
            else {
                keyPressed[i - 3][j] = 0;           //If the button was not pressed, we mark that that button has gone up
            }
            P2DIR ^= 1 << j;                        //set the col that was changed to input to output again
        }
        P2OUT ^= 1 << i;                            //set the row that was changed to LOW to HIGH again
    }

    //return btnVal;
    return '\0';                                    //No button pressed
}
