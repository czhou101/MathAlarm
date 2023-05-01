#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdint.h>
#include <stdio.h>

struct Equation {
    char  question[10];
    int   solution;
 };

/* Random number generation */
// -------------------------------------
void srand(uint16_t seed);
int rand();
struct Equation gen_equ();

#endif
