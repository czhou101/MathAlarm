#include <msp430.h> 
#include <adafruit_lcd_backpack_interface.h>
#include <keypad_interface.h>
#include <random.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int     second = 0;
int     minute = 0;
int     hour = 0;
char    hour_str[2];
char    min_str[2];
char    sec_str[2];
char    time_string[8];
char    count_down_time[8];
int     question_on = 0;
int     time_char_read = 0;
int     cursor_pos = 0;
char    read;
int     wait_for_start = 0;
int     start_timing = 0;
char    in_time[6] = {'0', '0', '0', '0', '0', '0'};
int     start_time_seed = 0;
int     read_solution_num = 0;              // the number of buttons pressed
char    in_solution[3] = {' ', ' ', ' '};   // string for storing the buttons input
char    equation_print[10];                 // string for showing the equation on the screen
int     solution_int = -1;                  // integer form of solution
char    solution_str_1[1];                  // string form of solution
char    solution_str_2[2];                  // string form of solution
char    solution_str_3[3];                  // string form of solution
int     attempt_num = 0;

struct Equation equ1;

int set_inputTime_pos(int charRead) {
    if (charRead <= 1) {
        return charRead;
    }
    else if (charRead <= 3) {
        return charRead + 1;
    }
    else if (charRead <= 5) {
        return charRead + 2;
    }
}

int set_inputSol_pos(int charRead) {
    return charRead + 6;
}

void initialize_all() {
    clear_screen();
    TA0CCR1 = 0;
    question_on = 0;
    time_char_read = 0;
    wait_for_start = 0;
    start_timing = 0;
    attempt_num = 0;
    hour = 0;
    minute = 0;
    second = 0;
    memset(in_time, '0',strlen(in_time));
    memset(hour_str, ' ',strlen(hour_str));
    memset(min_str, ' ',strlen(min_str));
    memset(sec_str, ' ',strlen(sec_str));
    memset(in_solution, ' ',strlen(in_solution));
}

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // timer1A for 1 second
    BCSCTL3 |= LFXT1S_2;                    // ACLK = VLO
    TA1CTL = TASSEL_1 + MC_1;               // ACLK, upmode
    TA1CCR0 = 12000;                        // Interrupt should happen every ~12 kHz / 12000
    TA1CCTL0 = CCIE;                        // CCR0 interrupt enabled

    // configure buzzer
    BCSCTL1   = CALBC1_1MHZ;                // Set DCO range
    DCOCTL    = CALDCO_1MHZ;                // Set DCO step and modulation

    P1DIR    |=  BIT6 + BIT7;               // Set P1.6, 1.7 to output-direction
    P1SEL    |=  BIT6;                      // Set selection register 1 for timer-function
    P1SEL2   &= ~BIT6;                      // Clear selection register 2 for timer-function
    P1OUT &= ~BIT7;                         // Set 1.7 to GND

    TA0CCTL1  = OUTMOD_7;                   // Reset/set
    TA0CTL    = (TASSEL_2 | MC_1);          // SMCLK, timer in up-mode
    TA0CCR0 = 200000;                       // Interrupt should happen every ~12 kHz / 12000
    TA0CCR1 = 0;                            // Duty-cycle

    // initialize keypad
    init_keypad();

    // initialize LCD
    init_spi();
    init_LCD();

    // initialize time string
    memset(hour_str, '0',strlen(hour_str));
    memset(min_str, '0',strlen(min_str));
    memset(sec_str, '0',strlen(sec_str));

    while (1) {
        __bis_SR_register(GIE);

        sprintf(time_string, "%c%c:%c%c:%c%c", in_time[0], in_time[1], in_time[2], in_time[3], in_time[4], in_time[5]);

        char read = check_keypress();

        // read input time
        if (time_char_read < 6) {
            cursor_pos = set_inputTime_pos(time_char_read);
            home();
            write_string(time_string);
            setCursor(cursor_pos, 0);
            blink();
            if (read != '\0') {
                if (time_char_read >= 1) {
                    if (read == '*') {
                        time_char_read -= 1;
                        in_time[time_char_read] = '0';
                    }
                }
                if (time_char_read == 2 | time_char_read == 4) {
                    // tens digit of minute and second, could only be <=5
                    if (read == '1' | read == '2' | read == '3' | read == '4' | read == '5' | read == '0') {
                        in_time[time_char_read] = read;
                        time_char_read += 1;
                    }
                }
                else {
                    // ones digit or the hour time
                    if (!(read == '*' | read == '#')) {
                        in_time[time_char_read] = read;
                        time_char_read += 1;
                    }
                }
            }
        }

        // finish reading input
        else {
            wait_for_start = 1;
        }

        // press # to start timer
        if (wait_for_start) {
            if (!question_on) {
                if (!start_timing) {

                    // display the line
                    noCursor();
                    home();
                    write_string(time_string);
                    setCursor(0, 1);
                    write_string("Press # to start");

                    // input * to change previous time
                    if (read == '*') {
                        time_char_read -= 1;
                        in_time[time_char_read] = '0';
                        wait_for_start = 0;
                        clear_screen();
                    }
                }


                if (read == '#') {
                    // start (restart) timing
                    start_timing = 1;

                    memset(hour_str, ' ',strlen(hour_str));
                    memset(min_str, ' ',strlen(min_str));
                    memset(sec_str, ' ',strlen(sec_str));

                    // get input time to number
                    memcpy(hour_str, in_time, 2);
                    hour = atoi(hour_str);

                    memcpy(min_str, in_time + 2, 2 /* Length */);
                    minute = atoi(min_str);

                    memcpy(sec_str, in_time + 4, 2 /* Length */);
                    second = atoi(sec_str);

                    clear_screen();
                    home();

                }
            }

        }

        if (start_timing) {
            // if input *, stop the timing and reset
            if (read == '*') {
                initialize_all();
            }
        }

        if (question_on) {
            // read result for the equation
            if (solution_int != equ1.solution) {

                // print the question and your answer
                 setCursor(0, 1);
                 write_string(equ1.question);
                 write_string(in_solution);
                 setCursor(set_inputSol_pos(read_solution_num), 1),
                 blink();

                 if (read != '\0') {

                     // when press # check the solution
                     if (read == '#') {
                         // convert the string to solution
                         if (read_solution_num == 1) {
                             memcpy(solution_str_1, in_solution, 1);
                             solution_int = atoi(solution_str_1);
                         }
                         else if (read_solution_num == 2) {
                             memcpy(solution_str_2, in_solution, 2);
                             solution_int = atoi(solution_str_2);
                         }
                         else if (read_solution_num == 3) {
                             memcpy(solution_str_3, in_solution, 3);
                             solution_int = atoi(solution_str_3);
                         }
                         else {
                             solution_int = -1;
                         }

                         // check if solution is correct
                         if (solution_int == equ1.solution) {
                             TA0CCR1 = 0;
                         } else {
                             TA0CCR1 += 150;
                             attempt_num += 1;
                         }

                         int j = 0;
                         for (j = 0; j < 3; j++) {
                             in_solution[j] = ' ';
                         }
                         read_solution_num = 0;

                         if (attempt_num == 5) {
                             srand(start_time_seed + 10);
                             equ1 = gen_equ();
                             TA0CCR1 = 1500;
                             attempt_num = 0;
                         }
                     }
                     else if (read == '*') {
                         // do nothing
                     }
                     else {
                         // number input
                         in_solution[read_solution_num] = read;
                         read_solution_num += 1;
                     }
                 }

            }

            if (solution_int == equ1.solution) {
                // solution correct, set everything to zero to restart
                initialize_all();
            }

        }




    }

	return 0;
}


// Timer A1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1_0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{

    if (start_timing) {

        if (second == 0 && minute == 0 && hour == 0) {
            start_timing = 0;
            srand(start_time_seed + 5);
            equ1 = gen_equ();
            question_on = 1;
            TA0CCR1 = 1500;
        }
        else {
            second -= 1;
            if (second == 0) {
                // all zero
                if (minute == 0 && hour == 0) {
                    start_timing = 0;
                    srand(start_time_seed + 5);
                    equ1 = gen_equ();
                    question_on = 1;
                    TA0CCR1 = 1500;
                }
                // minute == 0, hour != 0
                else if (minute == 0 && !(hour == 0)) {
                    hour -= 1;
                    minute = 59;
                    second = 59;
                }
                // minute != 0,
                else if (minute != 0) {
                    minute -= 1;
                    second = 59;
                }
            }
        }

        sprintf(count_down_time, "%02d:%02d:%02d", hour, minute, second);
        home();
        write_string(count_down_time);
    }
    else {
        start_time_seed += 1;
    }



    __bic_SR_register_on_exit(LPM3_bits);     // Clear LPM3 bits from 0(SR)
}
