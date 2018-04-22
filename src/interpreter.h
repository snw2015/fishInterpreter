#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <windows.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DEBUG
#define _DEBUG 0
#endif

typedef int bool;
#define True 1
#define False 0

#ifndef NULL
#define NULL 0
#endif

#define random(n) (rand() % (n))
#define currentStack (stacks[head - 1])


#define MAX_LINE_NUM 1024
#define MAX_LINE_LENGTH 1024
#define MAX_STACK_NUM 1024
#define MAX_STACK_SIZE 1024

#define MAX_INT 2147483647
#define MIN_INT -2147483648

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
typedef struct IntStack intStack;

	//something smells fishy... errors
#define ERR_DIV_ZERO 0
#define ERR_INVALID_INST 1
#define ERR_STACK_UNDERFLOW 2
#define ERR_STACKS_UNDERFLOW 3
#define ERR_INVALID_JUMP 4
//some interpreter-based errors
#define ERR_STACK_OVERFLOW 5
#define ERR_STACKS_OVERFLOW 6
//some errors may occurr before executing
#define ERR_LINE_2_LONG 7
#define ERR_LINE_2_MANY 8
#define ERR_MEMORY_LEAK 9
#define ERR_FILE_IO 10
#define ERR_FLAG 11

#ifdef __cplusplus
}
#endif


#endif /* INTERPRETER_H_ */