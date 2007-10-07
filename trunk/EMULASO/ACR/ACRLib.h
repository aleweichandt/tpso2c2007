/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: ACRLib.h
 * 
 * Historia
 * 23/09/2007	GT	Creacion
 *
 ------------------------------------------------------------------ 
 */
#ifndef ACRLIB_H_
#define ACRLIB_H_

#include "../incGeneral/incGeneral.h"
#include "../incGeneral/Estructuras/estructuras.h"
#include "../incGeneral/Sockets/conexiones.h"
#include "../incGeneral/Sockets/paquetesGeneral.h"
#include "../incGeneral/log.h"
#include "../incGeneral/libConf.h"
#include "MatrizRecursos.h"
#include "DatosRecurso.h"
#include "DatosPpcbACR.h"
#include <signal.h>

#define ALRM_T 1				/*Periodo en segundos para la alarma*/

#define SOCK_ESCUCHA 	0	/* Define los slots para los sockets */
#define SOCKS_OCUPADOS 	1
#define MALLOC_SOCKS_INI	1	/* Cantidad de mem para sockets reservada al inicio */


typedef struct 
{
	char 				sz_ACR_IP[LEN_IP];
	unsigned short int 	usi_ACR_Port;

	int					i_maxLifeTimePPCB;				/* tiempo maximo de vida de un ppcb en espera */
	
	char				sz_programPath[LEN_DIR];		/* path donde se ubican los programas *.emu */
	char				sz_userPath[LEN_DIR];
	
	tSocket** 			t_ListaSockets;
	unsigned int		ui_ultimoSocket;
	
	tSocket				*psocketADS;					/*Comunicacion con ADS*/
	t_nodo*				t_ListaSocketAdp;				/*Comunicacion con ADPs*/
	
} tACR;

/* "Objeto" Publico */
tACR 	ACR;

/* Prototipos */
int 	ACR_Init();
void 	ACR_Salir();
int 	ACR_LeerConfig();

void 	ACR_ProcesarSeniales( int senial );
void 	ACR_SenialTimer( int senial );

int 	ACR_ConectarACR();
void 	ACR_AceptarConexion( tSocket* sockIn );
void 	ACR_HandShake( tSocket* sockIn );
void 	ACR_AtenderADS ( tSocket *sockIn );
void 	ACR_AtenderADP ( tSocket *sockIn );
void 	ACR_AtenderPPCB ( tSocket *sockIn );
void 	ACR_CerrarConexion( tSocket *sockIn );
void 	ACR_DesconectarADS(tSocket *sockIn);
void 	ACR_DesconectarADP(tSocket *sockIn);
void 	ACR_DesconectarPPCB(tSocket *sockIn);


#endif /*ACRLIB_H_*/