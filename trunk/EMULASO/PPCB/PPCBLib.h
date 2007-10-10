/*
 ============================================================================
 Name        : PPCBLib.h
 Author      : Trimboli, Damián
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : App de PPCB
 ============================================================================
 */


#ifndef PPCBLIB_H_
#define PPCBLIB_H_

#include "../incGeneral/incGeneral.h"
#include "../incGeneral/Sockets/conexiones.h"
#include "../incGeneral/Sockets/paquetesGeneral.h"
#include "../incGeneral/log.h"
#include "../incGeneral/libConf.h"
#include "../incGeneral/Estructuras/Stack.h"

#include <signal.h>
#include <unistd.h>


#define SOCK_ESCUCHA 	0	/* Define los slots para los sockets */
#define SOCK_ACR 		2
#define SOCKS_OCUPADOS 	2
#define MALLOC_SOCKS_INI	2	/* Cantidad de mem para sockets reservada al inicio */

#define _TIMER_AVERAGE_		5 /*segs*/

typedef enum {
	PENDIENTE = 0,
	BLOQUEADO = 1,
	LISTO = 2,
	EJECUTANDO = 3
	
} tState;

typedef struct 
{
	char 				m_ACR_IP[LEN_IP];
	unsigned short int 	m_ACR_Port;
	
	char 				m_IP[LEN_IP];
	unsigned short int 	m_Port;

	tSocket** 			m_ListaSockets;
	unsigned int		m_ultimoSocket;
	
	/*Datos PCB*/
	
	long PPCB_ID;
	char User[LEN_USUARIO];
	char ProgName[LEN_COMANDO_EJEC];
	int SessionID;
	int IP;
	char ProgramPath[LEN_DIR];
	tState State;
	Stack stack;
	char Code[20][20];
	
	
} tPCB;




/* "Objeto" Publico */
tPCB 	PCB;

/* Prototipos */
int 	PCB_Init();
void 	PCB_Salir();
int 	PCB_LeerConfig();

void 	PCB_ProcesarSeniales( int senial );
void 	PCB_SenialTimer();

int 	PCB_ConectarConACR();
void 	PCB_AceptarConexion( tSocket* sockIn );
void 	PCB_ConfirmarConexion( tSocket* sockIn );
void 	PCB_HandShake( tSocket* sockIn );
void 	PCB_AtenderACR ( tSocket *sockIn );
void 	PCB_CerrarConexion( tSocket *sockIn );


#endif /*ADPLIB_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
