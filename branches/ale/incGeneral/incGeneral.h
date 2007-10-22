/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: incGeneral.h
 * 
 * Historia
 *		
 *
 ------------------------------------------------------------------ 
 */

#ifndef INCGENERAL_H_
#define INCGENERAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE	1
#define FALSE	0

#define 	OK		0
#define 	ERROR	-1 

#define CONN_BACKLOG		10

#define LEN_IP	16

/* Registro de los procesos */
#define _ID_MSHELL_	0
#define _ID_ADS_	1
#define _ID_ACR_	2
#define _ID_ADP_	3
#define _ID_PPCB_	4

#define _MSHELL_	"MSHELL"
#define _ADS_		"ADS"
/* ----------------------- */

#define bzero(donde, cuanto) memset(donde, '\0', cuanto); /* Como que no es ansi? */

/* Prototipos globales */
int getCadT(FILE *flujo, int fin, char *cad);

int ContarCharEnString( const char *szString, char cCharAContar );

#define _LARGO_CMD_	100

#endif /*INCGENERAL_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/