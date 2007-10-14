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

void push(Stack *S, char val)
{
    S->v[ S->top ] = val; 
   (S->top)++;    

}

char pop(Stack *S)
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
          printf("%c  ",S->v[i]); 
       }
       printf("\n");
    }
}
