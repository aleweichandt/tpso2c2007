/*
 ============================================================================
 Name        : Stack.c
 Author      : Trimboli, Damián
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : Stack source
 ============================================================================
 */

#include<stdio.h>
#include "Stack.h"

void push(Stack *S, int val)
{
    S->v[ S->top ] = val; 
   (S->top)++;    

}

int pop(Stack *S)
{
    (S->top)--;
    return (S->v[S->top]);

}

void init(Stack *S)
{
    S->top = 0;
}

int full(Stack *S)
{
    return (S->top >= TAM_MAX);
}

void MyStackPrint(Stack *S)
{
    int i;
    if (S->top == 0)
       printf("El stack esta vacio.\n");
    else
    {
       printf("Contenido: ");
       for (i=0;i<S->top;i++)
       {
          printf("%d  ",S->v[i]); 
       }
       printf("\n");
    }
}
