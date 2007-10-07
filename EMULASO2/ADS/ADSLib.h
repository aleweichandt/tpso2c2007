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
#include "./DatosUsuariosADS.h"
#include <signal.h>

#define SOCK_ESCUCHA 	0	/* Define los slots para los sockets */
#define SOCK_ACR		1		
#define SOCKS_OCUPADOS 	1
#define MALLOC_SOCKS_INI	1	/* Cantidad de mem para sockets reservada al inicio */
#define TERMINACION_ARCHIVO_CLAVE  ".key"


typedef struct 
{
	char 				m_ACR_IP[LEN_IP];
	unsigned short int 	m_ACR_Port;
	
	char 				m_IP[LEN_IP];
	unsigned short int 	m_Port;

	tSocket** 			m_ListaSockets;
	unsigned int		m_ultimoSocket;
	
	tListaUsuariosADS	m_ListaUsuarios;
	
	char				m_PathUsuarios[LEN_PATH_USUARIOS];
	
	char				m_PathClavesUsuarios[LEN_PATH_USUARIOS];
	
} tADS;

/* "Objeto" Publico */
tADS 	ADS;

/* Prototipos */
int 	ADS_Init();
void 	ADS_Salir();
int 	ADS_LeerConfig();

void 	ADS_ProcesarSeniales( int senial );
void 	ADS_SenialTimer();

int 	ADS_ConectarACR();
void 	ADS_ConfirmarConexion( tSocket* sockIn );
void 	ADS_AceptarConexion( tSocket* sockIn );
void 	ADS_HandShake( tSocket* sockIn );
void 	ADS_AtenderMSH ( tSocket *sockIn );
void 	ADS_AtenderACR ( tSocket *sockIn );
void 	ADS_CerrarConexion( tSocket *sockIn );

char*	ADS_BuscarUsuario(const char* szPathUsuarios, const char* userName);
void 	ADS_ExtraerUserName(char* szUserNAme, const char* szLinea);
char *  ADS_ValidarPassword(const char *szUsername, const char *szPassword, const char *szPathUsuarios);
void 	ADS_ExtraerPassword(char* szPassword, const char* szLinea);

int ADS_GetClaveByConnId(int connId, const char *pathClaves);


#endif /*ADSLIB_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
