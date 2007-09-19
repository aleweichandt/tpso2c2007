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

/*Posiciones para el parseo del mensaje*/
#define POS_PAQ_IP			0
#define POS_PAQ_ID_PORT		sizeof( paq->id.IP )
#define POS_PAQ_ID_PROC		POS_PAQ_ID_PORT + sizeof( paq->id.puerto )
#define POS_PAQ_ID_MSG		POS_PAQ_ID_PROC + sizeof( paq->id.id_Proceso )
#define POS_PAQ_ID_ID_UU	POS_PAQ_ID_MSG + sizeof( paq->id.id_Msg )
#define POS_PAQ_LEN_MSG		POS_PAQ_ID_ID_UU + sizeof( paq->id.UnUsed )
#define POS_PAQ_MSG			POS_PAQ_LEN_MSG + sizeof( paq->msg_len )


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
	
	PAQ_USR_NAME,
	PAQ_USR_PWD
	
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

char IS_PAQ_USR_NAME ( tPaquete *paq );
char IS_PAQ_USR_PWD ( tPaquete *paq );

/* --- */

/* Prototipos publicos */
void paquetes_destruir( tPaquete *paq );

tPaquete* paquetes_CharToPaq( const char* buffer  );
tPaquete* paquetes_PaqToPaq( const char* buffer  );


tPaquete* paquetes_newPaqPing( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqPong( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );


#endif /*PAQUETES__GRALH_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
