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
	PAQ_LOGIN_USR,
	PAQ_LOGIN_PWD,
	PAQ_LOGOUT,
	PAQ_EXIT,
	
	PAQ_USR_NAME,
	PAQ_USR_PWD,
	PAQ_EXEC_PROG,
	PAQ_PROG_EXECUTING,
	PAQ_NO_PROG,
	PAQ_USR_OK,
	PAQ_USR_ERROR,
	PAQ_PWD_OK,
	PAQ_PWD_ERROR,
	PAQ_PRINT,
	
	PAQ_END_SESION,
	PAQ_END_SESION_OK,
	
	/* 24-09-07:LAS: Registro ACR-ADP*/
	PAQ_GET_PERFORMANCE,
	PAQ_INFO_PERFORMANCE,
	PAQ_SUSPEND_PCB,
	PAQ_EXEC_PCB,
	
	/*01-10-07:LAS: Migracion*/
	PAQ_MIGRATE,
	PAQ_MIGRAR,
	PAQ_MIGRAR_OK,
	PAQ_MIGRAR_FAULT,
	PAQ_FIN_MIGRAR,
	PAQ_ARCHIVO,
	PAQ_KILL,
	/**/
	
	/*sol y dev de recursos*/
	PAQ_SOL,
	PAQ_DEV,
	
	/*RemainingTimeExecution*/
	PAQ_GET_REMAINING_TIME_EXECUTION,
	PAQ_INFO_REMAINING_TIME_EXECUTION
	
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

char IS_PAQ_USR_NAME ( tPaquete *paq );
char IS_PAQ_USR_PWD ( tPaquete *paq );

char IS_PAQ_END_SESION ( tPaquete *paq );
char IS_PAQ_END_SESION_OK( tPaquete *paq );

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
/**************			PAQ_PRINT				****************/
#define PRINT_LEN_NOM_PROG 		4
#define PRINT_LEN_MSG			44
#define PRINT_POS_ID_SESION 	0
#define PRINT_POS_NOM_PROG		sizeof(int)
#define PRINT_POS_MSG			PRINT_POS_NOM_PROG + PRINT_LEN_NOM_PROG

tPaquete* paquetes_newPaqPrint( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, 
		int idSesion, 
		char* szNomProg, 
		char* szMsg );
tPaquete* paquetes_ParsearPaqPrint( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, unsigned short int* puerto,
		int 	*idSesion, 
		char	*szNomProg, 
		char	*szMsg );
/***************************************************************/
/* ADS */
tPaquete* paquetes_newPaqUserNameOk( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqUserNameInvalido( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqPasswordOk( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqPasswordInvalido( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqEnd_Sesion( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char *idConeccion );
tPaquete* paquetes_newPaqEnd_Sesion_Ok( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char *idConeccion );

void paquetes_ParsearUserName( const char *msg, char *szUserName );
void paquetes_ParsearPassword(const char *msg, char *szPassword);

/**************			PAQ_EXEC_PROG				****************/
char IS_PAQ_EXEC_PROG ( tPaquete *paq );
tPaquete* paquetes_newPaqExecProg( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
								char szNomProg[LEN_COMANDO_EJEC], char szUsuario[LEN_USUARIO], int idSesion );
tPaquete* paquetes_ParsearPaqExecProg( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, 
										unsigned short int* puerto,char *szNomProg, char *szUsuario, int *idSesion );
char* paquetes_newPaqExecProgAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									char szNomProg[LEN_COMANDO_EJEC], char szUsuario[LEN_USUARIO], int idSesion );

/**************			PAQ_PROG_EXECUTING				****************/
char IS_PAQ_PROG_EXECUTING ( tPaquete *paq );
tPaquete* paquetes_newPaqProgExecuting( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
								char szNomProg[LEN_COMANDO_EJEC], int idSesion );
tPaquete* paquetes_ParsearPaqProgExecuting( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, 
										unsigned short int* puerto,char *szNomProg, int *idSesion );
char* paquetes_newPaqProgExecutingAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									char szNomProg[LEN_COMANDO_EJEC], int idSesion );

/**************			PAQ_NO_PROG				****************/
char IS_PAQ_NO_PROG ( tPaquete *paq );
tPaquete* paquetes_newPaqNoProg( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
								char szNomProg[LEN_COMANDO_EJEC], int idSesion );
tPaquete* paquetes_ParsearPaqNoProg( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, 
										unsigned short int* puerto,char *szNomProg, int *idSesion );
char* paquetes_newPaqNoProgAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									char szNomProg[LEN_COMANDO_EJEC], int idSesion );

/*----------------------- ADP -----------------------------------*/
#define POS_MAX_MEM			0
#define POS_CARGA_PROM		sizeof (int)
#define POS_CANT_PCB		POS_CARGA_PROM + sizeof (float) 

/* Funciones para identificar el paquete, seria conveniente ir agregando aca una funcion para cada paq */
char IS_PAQ_INFO_PERFORMANCE ( tPaquete *paq );
char IS_PAQ_EXEC_PCB ( tPaquete *paq );
char IS_PAQ_SUSPEND_PCB ( tPaquete *paq );

/* --- */

/* Prototipos publicos */
tPaquete* paquetes_newPaqInfoPerformance( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
											int nMaxMen, float fCargaProm, int CantPCB );

char* paquetes_newPaqInfoPerformanceAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
											int nMaxMen, float fCargaProm, int CantPCB );

tPaquete* paquetes_ParsearPaqInfoPerformance( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, 
											unsigned short int* puerto,
											int* nMaxMen, float* fCargaProm, int* CantPCB );
/* Para la planificacion */
tPaquete* paquetes_newPaqExecPCB( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
char* paquetes_newPaqExecPCBAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );

tPaquete* paquetes_newPaqSuspendPCB( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
char* paquetes_newPaqSuspendPCBAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
											
/*----------------------- ADP -----------------------------------*/

/*----------------------- ACR -----------------------------------*/

/* Funciones para identificar el paquete, seria conveniente ir agregando aca una funcion para cada paq */
char IS_PAQ_GET_PERFORMANCE ( tPaquete *paq );

/* --- */

/* Prototipos publicos */
tPaquete* paquetes_newPaqGetPerformance( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
char* paquetes_newPaqGetPerformanceAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );

/*----------------------- ACR -----------------------------------*/

/*--------------------- Migracion -------------------------------*/
#define MAX_PAQ_ARCH	512

typedef struct
{
	tIDMensaje id;
	long msg_len;
	char msg[ MAX_PAQ_ARCH +1 ];
}tPaqueteArch;

#define PAQUETE_ARCH_MAX_TAM		 sizeof(tPaqueteArch)


char IS_PAQ_MIGRAR ( tPaquete *paq );
char IS_PAQ_MIGRAR_OK ( tPaquete *paq );
char IS_PAQ_MIGRAR_FAULT ( tPaquete *paq );
char IS_PAQ_FIN_MIGRAR ( tPaqueteArch *paq );
char IS_PAQ_ARCHIVO ( tPaqueteArch *paq );

tPaquete* paquetes_newPaqMigrate( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									unsigned char IPDestino[4], unsigned short int puertoDestino );
tPaquete* paquetes_ParsearPaqMigrate( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, unsigned short int* puerto,
											unsigned char* IPDestino, unsigned short int* puertoDestino );
tPaquete* paquetes_newPaqMigrar( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, long lPCB_ID );
tPaquete* paquetes_newPaqMigrarOK( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqMigrarFault( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaqueteArch* paquetes_newPaqFinMigrar( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaqueteArch* paquetes_newArchivo( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char szArch[ MAX_PAQ_ARCH ] );

char* paquetes_newPaqMigrateAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									unsigned char IPDestino[4], unsigned short int puertoDestino );
char* paquetes_newPaqMigrarAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, long lPCB_ID );
char* paquetes_newPaqMigrarOKAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
char* paquetes_newPaqMigrarFaultAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
char* paquetes_newPaqFinMigrarAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
char* paquetes_newArchivoAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char szArch[ MAX_PAQ_ARCH ] );

char* paquetes_PaqArchToChar( tPaqueteArch* paq  );
void paquetes_Archdestruir( tPaqueteArch* paq  );
tPaqueteArch* paquetes_CharToPaqArch( const char* buffer  );

/*--------------------- Migracion -------------------------------*/
/*--------------------------paq_kill-----------------------------*/

char IS_PAQ_KILL ( tPaquete *paq );
tPaquete* paquetes_newPaqKill( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int pid[25] );

/*--------------------------paqs sol y dev-----------------------*/
#define SOLDEV_POS_PPCBID 	0
#define SOLDEV_POS_RECURSO	sizeof(int)

char IS_PAQ_SOL( tPaquete *paq );
char IS_PAQ_DEV( tPaquete *paq );
tPaquete* paquetes_newPaqSol( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int PPCB_id, tRecurso recursoSolicitado );
tPaquete* paquetes_newPaqDev( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int PPCB_id, tRecurso recursoDevuelto );
char* paquetes_newPaqSolAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int PPCB_id, tRecurso recursoSolicitado );
char* paquetes_newPaqDevAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int PPCB_id, tRecurso recursoDevuelto );
tPaquete* paquetes_ParsearSol( const char* buffer, unsigned char* IP[4], unsigned char* id_Proceso, unsigned short int* puerto, int* PPCB_id, tRecurso* recursoSolicitado );
tPaquete* paquetes_ParsearDev( const char* buffer, unsigned char* IP[4], unsigned char* id_Proceso, unsigned short int* puerto, int* PPCB_id, tRecurso* recursoDevuelto );

/*----------------------RemainingTimeExecution-------------------*/
#define RTM_POS_PPCBID 				0
#define RTM_POS_TIEMPO_RESTANTE 	sizeof(int)

char IS_PAQ_GET_REMAINING_TIME_EXECUTION( tPaquete *paq );
char IS_PAQ_INFO_REMAINING_TIME_EXECUTION( tPaquete *paq );
tPaquete* paquetes_newPaqGetRemainingTimeExecution( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
tPaquete* paquetes_newPaqInfoRemainingTimeExecution( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int PPCB_id, int tiempoRestante );
char* paquetes_newPaqGetRemainingTimeExecutionAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto );
char* paquetes_newPaqInfoRemainingTimeExecutionAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int PPCB_id, int tiempoRestante );
tPaquete* paquetes_ParsearInfoRemainingTimeExecution( const char* buffer, unsigned char* IP[4], unsigned char* id_Proceso, unsigned short int* puerto, int* PPCB_id, int* tiempoRestante );

#endif /*PAQUETES__GRALH_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
