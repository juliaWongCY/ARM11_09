#ifndef tower_stuct_h
#define tower_struct_h


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "SDL/SDL.h"

//There are three stacks and maximum 8 elements in each stack
int towers[3][6];

int currentTower;
int currentElem;

#define Max_elem 6
#define tower_num 3

void initialise(int);
void reset(int);
int isEmpty(int);
int getTopRingPos (int);
void push(int, int);
int peek(int);
int pop(int);
void move(int, int);
int check_win();

#endif
