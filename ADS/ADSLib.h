/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: ADSLib.c
 * 
 * Historia
 *	10-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */

#ifndef ADSLIB_H_
#define ADSLIB_H_

#include "../incGeneral/incGeneral.h"
#include "../incGeneral/Sockets/conexiones.h"
#include "../incGeneral/Sockets/paquetesGeneral.h"
#include "../incGeneral/log.h"
#include "../incGeneral/libConf.h"
#include <signal.h>

#define SOCK_ESCUCHA 	0	/* Define los slots para los sockets */
#define SOCKS_OCUPADOS 	1
#define MALLOC_SOCKS_INI	1	/* Cantidad de mem para sockets reservada al inicio */


typedef struct 
{
	char 				m_ACR_IP[LEN_IP];
	unsigned short int 	m_ACR_Port;
	
	char 				m_IP[LEN_IP];
	unsigned short int 	m_Port;

	tSocket** 			m_ListaSockets;
	unsigned int		m_ultimoSocket;
	
} tADS;

/* "Objeto" Publico */
tADS 	ADS;

/* Prototipos */
int 	ADS_Init();
void 	ADS_Salir();
int 	ADS_LeerConfig();

void 	ADS_ProcesarSeniales( int senial );
void 	ADS_SenialTimer();

int 	ADS_ConectarADS();
void 	ADS_AceptarConexion( tSocket* sockIn );
void 	ADS_HandShake( tSocket* sockIn );
void 	ADS_AtenderMSH ( tSocket *sockIn );
void 	ADS_AtenderACR ( tSocket *sockIn );
void 	ADS_CerrarConexion( tSocket *sockIn );


#endif /*ADSLIB_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
