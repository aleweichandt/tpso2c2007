/*
 ============================================================================
 Name        : Stack.h
 Author      : Trimboli, Damián
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : Header de Stack
 ============================================================================
 */

#define TAM_MAX 30

typedef struct
{
    int v[TAM_MAX];
    int top;
} Stack;

void push(Stack *S, int val);
int pop(Stack *S);
void init(Stack *S);
int full(Stack *S);
void StackPrint(Stack *S);

