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
#define _SIN_REPET_		1

#define CONN_BACKLOG		10

#define LEN_IP			16
#define LEN_DIR			30		/*23/09/2007	GT	Longitud de directorios*/ 
#define LEN_USUARIO		15		/*23/09/2007	GT	Longitud del nombre de usuario*/
#define LEN_COMANDO_EJEC 26		/*23/09/2007	GT	Longitud del comando que instacio un ppcb*/
#define LEN_NOM_ARCHIVO 16

#define LEN_PATH_USUARIOS	100

#define LEN_MAX_LINEA_ARCH_USUARIOS		100

#define LEN_USERNAME	20
#define LEN_PASSWORD	20
#define LEN_NOMBRE_MSHELL	20

/* Registro de los procesos */
#define _ID_MSHELL_	0
#define _ID_ADS_	1
#define _ID_ACR_	2
#define _ID_ADP_	3
#define _ID_PPCB_	4

#define _MSHELL_	"MSHELL"
#define _ADS_		"ADS"
#define _ADP_		"ADP"
#define _ACR_		"ACR"
#define _PPCB_		"PPCB"

/*23/09/2007	GT	Recursos*/
#define _SEP_RECURSO_			";"

/* ----------------------- */

#define bzero(donde, cuanto) memset(donde, '\0', cuanto); /* Como que no es ansi? */

typedef enum{
	Impresora = 0,
	Disco = 1,
	Cinta = 2
} tRecurso;

/* Prototipos globales */
int getCadT(FILE *flujo, int fin, char *cad);

int ContarCharEnString( const char *szString, char cCharAContar );

char* AplicarXorEnString(char *szStringOrigen, int clave);
int ReducirIP( const char* szIP, unsigned char* szIPReducido );
int AmpliarIP( const unsigned char* szIPReducido, char* szIP );

#define _LARGO_CMD_	100

void ArmarPathPCBConfig( char* szPathOut, long lpcb_id );/*02-10-07:LAS:*/

#endif /*INCGENERAL_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
