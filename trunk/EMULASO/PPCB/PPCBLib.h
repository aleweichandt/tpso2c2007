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
#include "../incGeneral/FindFile.h"
#include "../incGeneral/Estructuras/Stack.h"

#include <signal.h>
#include <unistd.h>


#define SOCK_ACR 		0
#define SOCK_ADP 		1

#define MALLOC_SOCKS_INI	1	/* Cantidad de mem para sockets reservada al inicio */

#define ALRM_T 1				/*Periodo en segundos para la alarma*/

/* LAS: Lo paso al incGeneral
typedef enum {
	PENDIENTE = 0,
	BLOQUEADO = 1,
	LISTO = 2,
	EJECUTANDO = 3
	
} tState;
*/
typedef struct 
{
	char 				m_ACR_IP[LEN_IP];
	unsigned short int 	m_ACR_Port;
	
	char 				m_ADP_IP[LEN_IP];
	unsigned short int 	m_ADP_Port;
	
	char 				m_IP[LEN_IP];
	unsigned short int 	m_Port;

	tSocket** 			m_ListaSockets;
	unsigned int		m_ultimoSocket;
	
	tSocket*			m_socketACR;
	tSocket*			m_socketADP;
	int					nIdProcesoPadre;
	int					argc;
	/*Datos PCB*/
	
	int PPCB_ID;
	char User[LEN_USUARIO];
	char ProgName[LEN_COMANDO_EJEC];
	int SessionID;
	int IP;
	int Mem;
	char ProgramPath[LEN_DIR];
	tState State;
	Stack stack;
	char Code[20][20];
	int	ultimaSentencia;
	int tiempoRestanteOper;
	
	
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
int 	PCB_ConectarConADP();
void 	PCB_AceptarConexion( tSocket* sockIn );
void 	PCB_ConfirmarConexion( tSocket* sockIn );
void 	PCB_HandShake( tSocket* sockIn );
void 	PCB_AtenderACR ( tSocket *sockIn );
void 	PCB_AtenderADP ( tSocket *sockIn );
void 	PCB_CerrarConexion( tSocket *sockIn );
void	PCB_ExecuteProgram(tSocket *sockIn);
int		PCB_RecursoFromFriendlyName(tRecurso* recurso, const char* param);
int 	PCB_ExecuteImpFinal(char *param);

#endif /*ADPLIB_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
