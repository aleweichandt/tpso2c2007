/*
 *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: paquetes.h
 * 
 * Historia
 *		
 * 	
 *
 ------------------------------------------------------------------ 
 */

#ifndef PAQUETES__GRALH_
#define PAQUETES__GRALH_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* htons, inet_ntoa */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../incGeneral.h"
#include "conexiones.h"

/* Constantes publicas */
#define PAQ_MSG_ERR  			-1


#define PAQ_LEN_MSGCTRL 50
/*#define LEN_USERNAME	50
#define LEN_NOMBRE_MSHELL	50 los pongo en incGeneral*/

/*Posiciones para el parseo del mensaje*/
#define POS_PAQ_IP			0
#define POS_PAQ_ID_PORT		sizeof( paq->id.IP )
#define POS_PAQ_ID_PROC		POS_PAQ_ID_PORT + sizeof( paq->id.puerto )
#define POS_PAQ_ID_MSG		POS_PAQ_ID_PROC + sizeof( paq->id.id_Proceso )
#define POS_PAQ_ID_ID_UU	POS_PAQ_ID_MSG + sizeof( paq->id.id_Msg )
#define POS_PAQ_LEN_MSG		POS_PAQ_ID_ID_UU + sizeof( paq->id.UnUsed )
#define POS_PAQ_MSG			POS_PAQ_LEN_MSG + sizeof( paq->msg_len )

#define POS_PAQ_ADS_NOMBRE_MSHELL	POS_PAQ_MSG + LEN_USERNAME
/*-----------------------------------------------------*/

/* Estructuras publicas */

typedef struct
{
	unsigned char IP[4];    /*4*/
	unsigned char id_Proceso;/*1*/
	unsigned char id_Msg;/*1*/
	unsigned short int puerto;   /*2*/
	unsigned char UnUsed[10];    /*10*/
	
}tIDMensaje; /*24-04-07: LAS: Cambio propuesto por Gonzalo*/

typedef struct
{
	tIDMensaje id;
	long msg_len;
	char msg[PAQ_LEN_MSGCTRL+1];
}tPaquete;

#define PAQUETE_MAX_TAM		 sizeof(tPaquete)

/* Registro de los ids de paquetes */
typedef enum 
{
	PAQ_PING,
	PAQ_PONG,
	PAQ_LOGIN_USR,
	PAQ_LOGIN_PWD,
	PAQ_LOGOUT,
	PAQ_EXIT,
	PAQ_EXEC_PROG,
	PAQ_USR_OK,
	PAQ_USR_ERROR,
	PAQ_PWD_OK,
	PAQ_PWD_ERROR,
	PAQ_PRINT
	
} tPaq_ids;
/* Fin de Registro de los ids de paquetes */

/* Funciones para identificar el Proceso que lo envia */
char IS_MSHELL_PAQ ( tPaquete *paq );
char IS_ADS_PAQ ( tPaquete *paq );
char IS_ACR_PAQ ( tPaquete *paq );
char IS_ADP_PAQ ( tPaquete *paq );
char IS_PPCB_PAQ ( tPaquete *paq );

/* Funciones para identificar el paquete, seria conveniente ir agregando aca una funcion para cada paq */
char IS_PAQ_PING ( tPaquete *paq );
char IS_PAQ_PONG ( tPaquete *paq );
char IS_PAQ_LOGIN_USR ( tPaquete *paq );
char IS_PAQ_LOGIN_PWD ( tPaquete *paq );
char IS_PAQ_LOGOUT ( tPaquete *paq );
char IS_PAQ_EXIT ( tPaquete *paq );
char IS_PAQ_EXEC_PROG ( tPaquete *paq );
char IS_PAQ_USR_OK ( tPaquete *paq );
char IS_PAQ_USR_ERROR ( tPaquete *paq );
char IS_PAQ_PWD_OK ( tPaquete *paq );
char IS_PAQ_PWD_ERROR ( tPaquete *paq );
char IS_PAQ_PRINT ( tPaquete *paq );
/* --- */

/* Prototipos publicos */
void paquetes_destruir( tPaquete *paq );

tPaquete* paquetes_CharToPaq( const char* buffer  );
tPaquete* paquetes_PaqToPaq( const char* buffer  );


tPaquete* paquetes_newPaqPing( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqPong( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
/* MShell */
tPaquete* paquetes_newPaqLogin_Usr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char user_Name[15] );
tPaquete* paquetes_newPaqLogin_Pwd( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char user_Password[15] );
tPaquete* paquetes_newPaqLogout( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqExit( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqExec_Prog( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char program_Name[30] );
tPaquete* paquetes_newPaqPrint( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char msg[30] );
/* ADS */
tPaquete* paquetes_newPaqUserNameOk( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqUserNameInvalido( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqPasswordOk( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqPasswordInvalido( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqADSLogout( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int idConeccion );

void paquetes_ParsearUserName( const char *msg, char *szUserName );
void paquetes_ParsearPassword(const char *msg, char *szPassword);

#endif /*PAQUETES__GRALH_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
