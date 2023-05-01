#include <random.h>
#include <stdlib.h>


uint16_t lfsr; // private variable which holds the state of the random number generation system

void srand(uint16_t seed) {
    int i;
    if (seed == 0)
        lfsr = 1;
    else
        lfsr = seed;

    // Churn the random number generator enough times to scramble the state
    // This is helpful if initializers are often small values, since rand()
    // is grabbing the least significant bits and the pseudorandom sequence
    // gets built from the most significant side.
    for (i = 0; i < 32; i++)
        rand();
}

int rand() {
    // Generates a random number between 0-9
    unsigned int bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5))  & 0x01;
    lfsr = (lfsr >> 1) | (bit << 15);
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5))  & 0x01;
    lfsr = (lfsr >> 1) | (bit << 15);
    return lfsr & 0x09;
}

struct Equation gen_equ() {
    struct Equation equ1;

    // get two numbers
    int num1_1 = rand(); // tens digit
    int num1_2 = rand(); // ones digit
    int num2_1 = rand();
    int num2_2 = rand();

    int num1 = num1_1 * 10 + num1_2;
    int num2 = num2_1 * 10 + num2_2;
    // add or subtract
    int operator = rand();
    if (operator <= 4) {
        // +
        sprintf(equ1.question, "%2d+%2d=", num1, num2);
        equ1.solution = num1 + num2;
    } else {
        // -
        if (num1 > num2) {
            sprintf(equ1.question, "%2d-%2d=", num1, num2);
        } else {
            sprintf(equ1.question, "%2d-%2d=", num2, num1);
        }
        equ1.solution = abs(num1 - num2);
    }

    return equ1;
}

