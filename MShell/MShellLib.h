/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: MShellLib.h
 * 
 * Historia
 *	03-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */

#ifndef MSHELLLIB_H_
#define MSHELLLIB_H_

#include "../incGeneral/incGeneral.h"
#include "../incGeneral/Sockets/conexiones.h"
#include "../incGeneral/Sockets/paquetesGeneral.h"
#include "../incGeneral/log.h"
#include "../incGeneral/ventanas.h"
#include "../incGeneral/libConf.h"
#include <signal.h>

#define SOCK_TECLADO 	0	/* Define los slots para los sockets */
#define SOCK_ADS 		1
#define SOCKS_OCUPADOS 	2
#define MALLOC_SOCKS_INI	2	/* Cantidad de mem para sockets reservada al inicio */


typedef struct 
{
	char 				m_ADS_IP[LEN_IP];
	unsigned short int 	m_ADS_Port;
	
	tSocket** 			m_ListaSockets;
	unsigned int		m_ultimoSocket;
	
	tVentana			*m_pwRemoto;
	tVentana			*m_pwLocal;
} tMShell;

/* "Objeto" Publico */
tMShell 	MShell;

/* Prototipos */
int 	MSH_Init();
void 	MSH_Salir();
int 	MSH_LeerConfig();
void 	MSH_CrearGraficos( int activarGraficos );

void 	MSH_ProcesarSeniales( int senial );
void 	MSH_ProcesarTeclado(tSocket*);				/* Procesar input de usuario */
void 	MSH_SenialTimer();

int 	MSH_ConectarADS();
void 	MSH_ConfirmarConexion( tSocket* sockIn );
void 	MSH_AtenderADS ( tSocket *sockIn );

int 	MSH_Login_Send(char msj[15]);
int 	MSH_Exec_Prog(char prog[30]);
int 	MSH_Logout();
#endif /*MSHELLLIB_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
