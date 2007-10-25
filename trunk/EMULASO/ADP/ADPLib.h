/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: ADPLib.c
 * 
 * Historia
 *	13-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */

#ifndef ADPLIB_H_
#define ADPLIB_H_
#include "../incGeneral/incGeneral.h"
#include "../incGeneral/Sockets/conexiones.h"
#include "../incGeneral/Sockets/paquetesGeneral.h"
#include "../incGeneral/log.h"
#include "../incGeneral/libConf.h"
#include <signal.h>
#include "DatosPCBs.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define SOCK_ESCUCHA 	0	/* Define los slots para los sockets */
#define SOCK_ACR 		1
#define SOCKS_OCUPADOS 	2
#define MALLOC_SOCKS_INI	2	/* Cantidad de mem para sockets reservada al inicio */

#define _TIMER_AVERAGE_		5 /*segs*/

typedef struct 
{
	char 				m_ACR_IP[LEN_IP];
	unsigned short int 	m_ACR_Port;
	
	char 				m_IP[LEN_IP];
	unsigned short int 	m_Port;

	tSocket** 			m_ListaSockets;
	unsigned int		m_ultimoSocket;
	
	/*Planificacion*/
	tListaPCB	m_LPL;
	tListaPCB	m_LPB;
	tListaPCB	m_LPE;
	
	int			m_Q;
	int			m_nLimite1,
				m_nLimite2;
	/*Memoria*/
	int			m_nMemMax,
				m_nMemDisp;	

	
} tADP;

/* "Objeto" Publico */
tADP 	ADP;

/* Prototipos */
int 	ADP_Init();
void 	ADP_Salir();
int 	ADP_LeerConfig();

void 	ADP_ProcesarSeniales( int senial );
void 	ADP_SenialTimer();

int 	ADP_ConectarConACR();
void 	ADP_AceptarConexion( tSocket* sockIn );
void 	ADP_ConfirmarConexion( tSocket* sockIn );
void 	ADP_HandShake( tSocket* sockIn );
void 	ADP_AtenderPCB ( tSocket *sockIn );
void 	ADP_AtenderACR ( tSocket *sockIn );
void 	ADP_CerrarConexion( tSocket *sockIn );

float	ADP_CalcularCargaPromReal();
int		ADP_EstoyCargado();
void	ADP_PasarMsgAlACR();
void	ADP_PasarMsgAlPCB();
void	ADP_RealizarInforme();

/* - Migracion - */
void 	ADP_RecibirArchivo( tSocket *sockIn );
int 	ADP_ForkearPCB( long lpcbid, long* plpid );
int		ADP_CrearPCB( long lpcbid, tSocket* pSock, int nMemoria, long pid );
int		ADP_MigrarPCBPesado();

/* - Planificacion - */
void 	ADP_ActivarAlarma();
void 	ADP_DesactivarAlarma();
int 	ADP_SeCumplioQ();
int 	ADP_SeCumplioTimerAverage();
void 	ADP_Dispatcher(int n);
int 	ADP_InformarSuspencion();
void 	ADP_InformarReanudacion();
int 	ADP_PasarDeLPLaLPE();

/* - liberar recursos - */
void 	ADP_LiberarRecursos(int pid);


#endif /*ADPLIB_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
