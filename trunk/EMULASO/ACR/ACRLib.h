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
#include "../incGeneral/FindFile.h"
#include "MatrizRecursos.h"
#include "DatosRecurso.h"
#include "DatosPpcbACR.h"
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ALRM_T 6				/*Periodo en segundos para la alarma*/

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
	t_nodo				*t_ListaSocketAdp;				/*Comunicacion con ADPs*/
	tListaPpcbAcr		t_ListaPpcbPend;				/*Comunicación con PPCBs*/
	
} tACR;

/* "Objeto" Publico */
tACR 	ACR;

long	lContProcesos;

/* Prototipos */
int 	ACR_Init();
void 	ACR_Salir();
int 	ACR_LeerConfig();
int 	ACR_LeerConfigRuntime();

void 	ACR_ProcesarSeniales( int senial );
void 	ACR_SenialTimer( int senial );
void 	ACR_SacarTimer(void);
void 	ACR_PonerTimer(void);

int 	ACR_ConectarACR();
void 	ACR_AceptarConexion( tSocket* sockIn );
void 	ACR_HandShake( tSocket* sockIn );
void 	ACR_AtenderADS ( tSocket *sockIn );
void 	ACR_AtenderADP ( tSocket *sockIn );
void 	ACR_AtenderPPCB ( tSocket *sockIn );
void	ACR_RecibirArchivo( tSocket *sockIn );
void 	ACR_ControlarPendientes(void);
void	ACR_DeterminarNodo(tPpcbAcr* tPpcb);
void	ACR_MigrarProceso( tPpcbAcr* tPpcb, unsigned char ipIdeal[4], unsigned short puertoIdeal);
int		ACR_ForkPPCB( long lpcbid );
int 	ACR_ForkPPCBInicial( long lpcb_id, char szNomProg[LEN_COMANDO_EJEC], char szUsuario[LEN_USUARIO], int idSesion);
int		ACR_CrearPPCB( long lpcbid, int pidChild );
int		ACR_CrearPPCBInicial( long lpcb_id, int pidChild, char szNomProg[LEN_COMANDO_EJEC], char szUsuario[LEN_USUARIO], int idSesion);

void 	ACR_CerrarConexion( tSocket *sockIn );
void 	ACR_DesconectarADS(tSocket *sockIn);
void 	ACR_DesconectarADP(tSocket *sockIn);
void 	ACR_DesconectarPPCB(tSocket *sockIn);


#endif /*ACRLIB_H_*/
