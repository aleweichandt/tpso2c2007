/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: paquetes.c
 * 
 * Historia
 *		
 *  
 *
 ------------------------------------------------------------------ 
 */

#include "paquetesGeneral.h"

char g_Paq[PAQUETE_MAX_TAM];
char g_PaqArch[PAQUETE_ARCH_MAX_TAM];/*01-10-07: LAS:*/


/* Protos privado */
tPaquete* paquetes_Crear();
void paquetes_CargarIdMSg( tPaquete *paq, unsigned char IP[4], unsigned char id_Proceso, unsigned char id_Msg,
								unsigned short int puerto );
/*01-10-07: LAS: BEG*/
tPaqueteArch* paquetes_CrearArch();
void paquetes_CargarIdMSgArch( tPaqueteArch *paq, unsigned char IP[4], unsigned char id_Proceso, unsigned char id_Msg,
								unsigned short int puerto );
tPaquete* CrearPaqSinBody( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, unsigned char id_Msg );
/*01-10-07: LAS:`END*/

/*----------------------------------------*/
tPaquete* paquetes_Crear()
{
	tPaquete *pPaq;
	
	if ( !(pPaq = malloc( sizeof(tPaquete) ) ) )
		return NULL;
	
	memset( pPaq, 0, sizeof(tPaquete) );
	
	return pPaq;
}

/*------------------------------------------------------------------------------------------------------------*/
void paquetes_CargarIdMSg( tPaquete *paq, unsigned char IP[4], unsigned char id_Proceso, unsigned char id_Msg,
								unsigned short int puerto )
{
	memcpy( paq->id.IP, IP, sizeof(unsigned char[4]) );
	paq->id.id_Proceso = id_Proceso;
	paq->id.id_Msg = id_Msg;
	paq->id.puerto = puerto;
}

/*------------------------------------------------------------------------------------------------------------*/
void paquetes_CargarMSg( tPaquete *paq, char *msg )
{
	strcpy( paq->msg, msg );
}

/*-------------------------------------------------------*/

char IS_MSHELL_PAQ ( tPaquete *paq ) { return (paq->id.id_Proceso == _ID_MSHELL_); }
char IS_ADS_PAQ ( tPaquete *paq )  { return (paq->id.id_Proceso == _ID_ADS_); }
char IS_ACR_PAQ ( tPaquete *paq ) { return (paq->id.id_Proceso == _ID_ACR_); }
char IS_ADP_PAQ ( tPaquete *paq ) { return (paq->id.id_Proceso == _ID_ADP_); }
char IS_PPCB_PAQ ( tPaquete *paq ) { return (paq->id.id_Proceso == _ID_PPCB_); }


char IS_PAQ_PING ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_PING); }

char IS_PAQ_PONG ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_PONG); }

char IS_PAQ_LOGIN_USR ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_LOGIN_USR); }
char IS_PAQ_LOGIN_PWD ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_LOGIN_PWD); }
char IS_PAQ_LOGOUT ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_LOGOUT); }
char IS_PAQ_EXIT ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_EXIT); }
char IS_PAQ_EXEC_PROG ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_EXEC_PROG); }
char IS_PAQ_USR_OK ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_USR_OK); }
char IS_PAQ_PWD_OK ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_PWD_OK); }
char IS_PAQ_USR_ERROR ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_USR_ERROR); }
char IS_PAQ_PWD_ERROR ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_PWD_ERROR); }
char IS_PAQ_PRINT ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_PRINT); }
char IS_PAQ_USR_NAME ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_USR_NAME); }
char IS_PAQ_USR_PWD ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_USR_PWD); }
char IS_PAQ_END_SESION ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_END_SESION); }
char IS_PAQ_END_SESION_OK ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_END_SESION_OK); }

/*******************************************************************/
void paquetes_destruir( tPaquete* paq  )
{
	if ( paq )
		free( paq );
}

/*******************************************************************/
tPaquete* paquetes_CharToPaq( const char* buffer  )
{
	tPaquete *paq;
	char szAux[ PAQUETE_MAX_TAM ];
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;
	
	memcpy( &paq->id.IP, &(buffer[ POS_PAQ_IP ]), sizeof( paq->id.IP ) );
	memcpy( &paq->id.puerto, &(buffer[ POS_PAQ_ID_PORT ]), sizeof( paq->id.puerto ) );
	memcpy( &paq->id.id_Proceso, &(buffer[ POS_PAQ_ID_PROC ]), sizeof( paq->id.id_Proceso ) );
	memcpy( &paq->id.id_Msg,  &(buffer[ POS_PAQ_ID_MSG ]), sizeof( paq->id.id_Msg ) );
	memcpy( &paq->id.UnUsed, &(buffer[ POS_PAQ_ID_ID_UU ]), sizeof( paq->id.UnUsed ) );
	
	memcpy( &paq->msg_len, &(buffer[ POS_PAQ_LEN_MSG ]), sizeof( paq->msg_len ) );
	memcpy( &paq->msg, &(buffer[ POS_PAQ_MSG ]), sizeof( paq->msg ) );
	
	return paq;
}

/*******************************************************************/
char* paquetes_PaqToChar( tPaquete* paq  )
{
	memset( g_Paq, 0, PAQUETE_MAX_TAM );
	
	memcpy( &g_Paq, paq->id.IP, sizeof( paq->id.IP ) );
	memcpy( &(g_Paq[ POS_PAQ_ID_PORT ]), &paq->id.puerto, sizeof( paq->id.puerto ) );
	memcpy( &(g_Paq[ POS_PAQ_ID_PROC ]), &paq->id.id_Proceso, sizeof( paq->id.id_Proceso ) );
	memcpy( &(g_Paq[ POS_PAQ_ID_MSG ]), &paq->id.id_Msg, sizeof( paq->id.id_Msg ) );
	memcpy( &(g_Paq[ POS_PAQ_ID_ID_UU ]), &paq->id.UnUsed, sizeof( paq->id.UnUsed ) );
	
	memcpy( &(g_Paq[ POS_PAQ_LEN_MSG ]), &paq->msg_len, sizeof( paq->msg_len ) );
	memcpy( &(g_Paq[ POS_PAQ_MSG ]), &paq->msg, sizeof( paq->msg ) );

	return g_Paq;	
}

/*******************************************************************/
tPaquete* paquetes_newPaqPing( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_PING, puerto );
	
	return paq;		
}

/*******************************************************************/
tPaquete* paquetes_newPaqPong( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_PONG, puerto );
	
	return paq;		
}
/*******************************************************************/
tPaquete* paquetes_newPaqUserNameOk( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_USR_OK, puerto );
	
	return paq;		
}
/*******************************************************************/
tPaquete* paquetes_newPaqPasswordOk( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
		
		if ( !(paq = paquetes_Crear() ) )
			return NULL;

		paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_PWD_OK, puerto );
		
		return paq;
}
/*******************************************************************/
tPaquete* paquetes_newPaqPasswordInvalido( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
		
		if ( !(paq = paquetes_Crear() ) )
			return NULL;

		paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_PWD_ERROR, puerto );
		
		return paq;
}
/*******************************************************************/
tPaquete* paquetes_newPaqEnd_Sesion( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char *idConeccion )
{
	tPaquete *paq;
		
		if ( !(paq = paquetes_Crear() ) )
			return NULL;

		paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_END_SESION, puerto );
		paquetes_CargarMSg( paq, idConeccion);
		
		return paq;
}
/*******************************************************************/
tPaquete* paquetes_newPaqEnd_Sesion_Ok( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char *idConeccion )
{
	tPaquete *paq;
		
		if ( !(paq = paquetes_Crear() ) )
			return NULL;

		paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_END_SESION_OK, puerto );
		paquetes_CargarMSg( paq, idConeccion);
		
		return paq;
}
/*******************************************************************/
tPaquete* paquetes_newPaqUserNameInvalido( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_USR_ERROR, puerto );
	
	return paq;		
}
/**********************************************************************/
void paquetes_ParsearUserName( const char *msg, char *szUserName )
{
	tPaquete *paq;
	memcpy( szUserName,		&(msg[POS_PAQ_MSG]), 	LEN_USERNAME );
}
/**********************************************************************/
void paquetes_ParsearPassword(const char *msg, char *szPassword)
{
	tPaquete *paq;
	memcpy( szPassword,		&(msg[POS_PAQ_MSG]), 	LEN_PASSWORD );
}

/*******************************************************************/
tPaquete* paquetes_newPaqLogin_Usr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char user_Name[15] )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_LOGIN_USR, puerto );
	paquetes_CargarMSg( paq, user_Name );
	
	return paq;		
}

/*******************************************************************/
tPaquete* paquetes_newPaqLogin_Pwd( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char user_Password[15] )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_LOGIN_PWD, puerto );
	paquetes_CargarMSg( paq, user_Password );
	
	return paq;		
}

/*******************************************************************/
tPaquete* paquetes_newPaqLogout( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_LOGOUT, puerto );
	
	return paq;		
}

/*******************************************************************/
tPaquete* paquetes_newPaqExit( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_EXIT, puerto );
	
	return paq;		
}

/*******************************************************************/
tPaquete* paquetes_newPaqExec_Prog( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char program_Name[30] )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_EXEC_PROG, puerto );
	paquetes_CargarMSg( paq, program_Name );
	
	return paq;		
}

/*******************************************************************/

tPaquete* paquetes_newPaqPrint( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, 
		int idSesion, 
		char* szNomProg, 
		char* szMsg)
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_PRINT, puerto );
	memcpy( &(paq->msg[PRINT_POS_ID_SESION]), &idSesion, sizeof( int )); 
	memcpy( &(paq->msg[PRINT_POS_NOM_PROG]), szNomProg, PRINT_LEN_NOM_PROG );
	memcpy( &(paq->msg[PRINT_POS_MSG]), szMsg , PRINT_LEN_MSG );
	
	return paq;		
}
/*******************************************************************/
tPaquete* paquetes_ParsearPaqPrint( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, unsigned short int* puerto,
		int 	*idSesion, 
		char	*szNomProg, 
		char	*szMsg )
{
	tPaquete *paq;
		
		if ( !(paq = paquetes_CharToPaq( Buffer ) ) )
			return NULL;
		
		memcpy( IP, &(paq->id.IP), sizeof(char[4]) );
		memcpy( id_Proceso, &(paq->id.id_Proceso), sizeof(unsigned char) );
		memcpy( puerto, &(paq->id.puerto), sizeof(unsigned short int) );
		memcpy( idSesion, &(paq->msg[PRINT_POS_ID_SESION]),  sizeof( int )  ); 
		memcpy( szNomProg, &(paq->msg[PRINT_POS_NOM_PROG]),  PRINT_LEN_NOM_PROG  );
		memcpy( szMsg, &(paq->msg[PRINT_POS_MSG]), PRINT_LEN_MSG );
		
		return paq;		
}

/**************			PAQ_EXEC_PROG				****************/
/* 07/10/2007	GT	Como ya estaba programado, lo comento 
char IS_PAQ_EXEC_PROG ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_EXEC_PROG); }*/

#define EP_POS_NOM_PROG 0
#define EP_POS_USR 		EP_POS_NOM_PROG + LEN_COMANDO_EJEC
#define EP_POS_ID_SS	EP_POS_USR +LEN_USUARIO


tPaquete* paquetes_newPaqExecProg( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
								char szNomProg[LEN_COMANDO_EJEC], char szUsuario[LEN_USUARIO], int idSesion )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_EXEC_PROG, puerto );
	
	memcpy( &(paq->msg[EP_POS_NOM_PROG]), szNomProg, LEN_COMANDO_EJEC ); 
	memcpy( &(paq->msg[EP_POS_USR]), szUsuario, LEN_USUARIO );
	memcpy( &(paq->msg[EP_POS_ID_SS]), &idSesion, sizeof( int ) );
	
	return paq;		
}


tPaquete* paquetes_ParsearPaqExecProg( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, 
										unsigned short int* puerto,
										char *szNomProg, char *szUsuario, int *idSesion )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_CharToPaq( Buffer ) ) )
		return NULL;
	
	memcpy( IP, &(paq->id.IP), sizeof(char[4]) );
	memcpy( id_Proceso, &(paq->id.id_Proceso), sizeof(unsigned char) );
	memcpy( puerto, &(paq->id.puerto), sizeof(unsigned short int) );
	memcpy( szNomProg, &(paq->msg[EP_POS_NOM_PROG]),  LEN_COMANDO_EJEC  ); 
	memcpy( szUsuario, &(paq->msg[EP_POS_USR]),  LEN_USUARIO  );
	memcpy( idSesion, &(paq->msg[EP_POS_ID_SS]), sizeof( int ) );
	
	return paq;		

}

char* paquetes_newPaqExecProgAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									char szNomProg[LEN_COMANDO_EJEC], char szUsuario[LEN_USUARIO], int idSesion )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqExecProg(IP,id_Proceso,puerto,szNomProg, szUsuario,idSesion)) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}

#undef EP_POS_NOM_PROG
#undef EP_POS_USR
#undef EP_POS_ID_SS


/**************			PAQ_PROG_EXECUTING				****************/

char IS_PAQ_PROG_EXECUTING ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_PROG_EXECUTING); }

#define PE_POS_NOM_PROG 0
#define PE_POS_ID_SS 	PE_POS_NOM_PROG + LEN_COMANDO_EJEC

tPaquete* paquetes_newPaqProgExecuting( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
								char szNomProg[LEN_COMANDO_EJEC], int idSesion )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_PROG_EXECUTING, puerto );
	
	memcpy( &(paq->msg[PE_POS_NOM_PROG]), szNomProg, LEN_COMANDO_EJEC  ); 
	memcpy( &(paq->msg[PE_POS_ID_SS]), &idSesion, sizeof( int ) );
	
	return paq;		
}

tPaquete* paquetes_ParsearPaqProgExecuting( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, 
										unsigned short int* puerto,char *szNomProg, int *idSesion )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_CharToPaq( Buffer ) ) )
		return NULL;
	
	memcpy( IP, &(paq->id.IP), sizeof(char[4]) );
	memcpy( id_Proceso, &(paq->id.id_Proceso), sizeof(unsigned char) );
	memcpy( puerto, &(paq->id.puerto), sizeof(unsigned short int) );
	memcpy( szNomProg, &(paq->msg[PE_POS_NOM_PROG]),  LEN_COMANDO_EJEC ); 
	memcpy( idSesion, &(paq->msg[PE_POS_ID_SS]), sizeof( int ) );
	
	return paq;		

}											
										
char* paquetes_newPaqProgExecutingAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									char szNomProg[LEN_COMANDO_EJEC], int idSesion )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqProgExecuting(IP,id_Proceso,puerto,szNomProg,idSesion)) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;										
}

/**************			PAQ_NO_PROG				****************/

char IS_PAQ_NO_PROG ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_NO_PROG); }

tPaquete* paquetes_newPaqNoProg( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
								char szNomProg[LEN_COMANDO_EJEC], int idSesion )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_NO_PROG, puerto );
	
	memcpy( &(paq->msg[PE_POS_NOM_PROG]), szNomProg, LEN_COMANDO_EJEC  ); 
	memcpy( &(paq->msg[PE_POS_ID_SS]), &idSesion, sizeof( int ) );
	
	return paq;		
}
								
tPaquete* paquetes_ParsearPaqNoProg( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, 
										unsigned short int* puerto,char *szNomProg, int *idSesion )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_CharToPaq( Buffer ) ) )
		return NULL;
	
	memcpy( IP, &(paq->id.IP), sizeof(char[4]) );
	memcpy( id_Proceso, &(paq->id.id_Proceso), sizeof(unsigned char) );
	memcpy( puerto, &(paq->id.puerto), sizeof(unsigned short int) );
	memcpy( szNomProg, &(paq->msg[PE_POS_NOM_PROG]),  LEN_COMANDO_EJEC ); 
	memcpy( idSesion, &(paq->msg[PE_POS_ID_SS]), sizeof( int ) );
	
	return paq;		
}

char* paquetes_newPaqNoProgAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									char szNomProg[LEN_COMANDO_EJEC], int idSesion )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqNoProg(IP,id_Proceso,puerto,szNomProg,idSesion)) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;										
}

#undef PE_POS_NOM_PROG
#undef PE_POS_ID_SS

/*----------------------- ADP -----------------------------------*/
char IS_PAQ_INFO_PERFORMANCE ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_INFO_PERFORMANCE); }
char IS_PAQ_SUSPEND_PCB ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_SUSPEND_PCB); }
char IS_PAQ_EXEC_PCB ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_EXEC_PCB); }

/*******************************************************************************************/
tPaquete* paquetes_newPaqExecPCB( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_EXEC_PCB, puerto );
	
	return paq;		
}

/*******************************************************************************************/
char* paquetes_newPaqExecPCBAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqExecPCB(IP,id_Proceso, puerto )) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}

/*******************************************************************************************/
tPaquete* paquetes_newPaqSuspendPCB( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_SUSPEND_PCB, puerto );
	
	return paq;		
}

/*******************************************************************************************/
char* paquetes_newPaqSuspendPCBAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqSuspendPCB(IP,id_Proceso, puerto )) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}

/*******************************************************************************************/
tPaquete* paquetes_newPaqInfoPerformance( unsigned char IP[4], unsigned char id_Proceso, 
											unsigned short int puerto,
											int nMaxMen, float fCargaProm, int CantPCB )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_INFO_PERFORMANCE, puerto );

	paq->msg_len = PAQ_LEN_MSGCTRL;
	
	memcpy( &(paq->msg[POS_MAX_MEM]), &nMaxMen, sizeof( nMaxMen ) ); 
	memcpy( &(paq->msg[POS_CARGA_PROM]), &fCargaProm, sizeof( fCargaProm ) );
	memcpy( &(paq->msg[POS_CANT_PCB]), &CantPCB, sizeof( CantPCB ) );
	
	
	return paq;		

}

/*******************************************************************************************/
char* paquetes_newPaqInfoPerformanceAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
											int nMaxMen, float fCargaProm, int CantPCB )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqInfoPerformance(IP,id_Proceso, puerto,nMaxMen, fCargaProm,CantPCB )) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}

/*******************************************************************************************/
tPaquete* paquetes_ParsearPaqInfoPerformance( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, 
											unsigned short int* puerto,
											int* nMaxMen, float* fCargaProm, int* CantPCB )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_CharToPaq( Buffer ) ) )
		return NULL;
	
	memcpy( IP, &(paq->id.IP), sizeof(unsigned char[4]) );
	memcpy( id_Proceso, &(paq->id.id_Proceso), sizeof(unsigned char) );
	memcpy( puerto, &(paq->id.puerto), sizeof(unsigned short int) );
	memcpy( nMaxMen, &(paq->msg[POS_MAX_MEM]), sizeof( int ) ); 
	memcpy( fCargaProm, &(paq->msg[POS_CARGA_PROM]), sizeof( float ) );
	memcpy( CantPCB, &(paq->msg[POS_CANT_PCB]), sizeof( int ) );
	
	return paq;		

}
/*----------------------- ADP -----------------------------------*/

/*----------------------- ACR -----------------------------------*/


/*******************	PAQ_GET_PERFORMANCE		*******************/
char IS_PAQ_GET_PERFORMANCE ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_GET_PERFORMANCE); }

/*******************************************************************************************/
tPaquete* paquetes_newPaqGetPerformance( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_GET_PERFORMANCE, puerto );
	
	return paq;		
}

/*******************************************************************************************/
char* paquetes_newPaqGetPerformanceAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqGetPerformance(IP,id_Proceso, puerto )) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}
/*----------------------- ACR -----------------------------------*/


/*--------------------- Migracion -------------------------------*/

/*----------------------------------------*/
tPaqueteArch* paquetes_CrearArch()
{
	tPaqueteArch *pPaq;
	
	if ( !(pPaq = malloc( sizeof(tPaqueteArch) ) ) )
		return NULL;
	
	memset( pPaq, ' ', sizeof(tPaqueteArch) );
	
	return pPaq;
}

/*------------------------------------------------------------------------------------------------------------*/
void paquetes_CargarIdMSgArch( tPaqueteArch *paq, unsigned char IP[4], unsigned char id_Proceso, unsigned char id_Msg,
								unsigned short int puerto )
{
	memcpy( paq->id.IP, IP, 4 );
	paq->id.id_Proceso = id_Proceso;
	paq->id.id_Msg = id_Msg;
	paq->id.puerto = puerto;
}

/*---------------------------------------------------------------------------------------------------------------------*/
tPaquete* CrearPaqSinBody( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, unsigned char id_Msg )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, id_Msg, puerto );
	
	return paq;		
}

char IS_PAQ_MIGRAR ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_MIGRAR); }
char IS_PAQ_MIGRAR_OK ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_MIGRAR_OK); }
char IS_PAQ_MIGRAR_FAULT ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_MIGRAR_FAULT); }
char IS_PAQ_FIN_MIGRAR ( tPaqueteArch *paq ) { return (paq->id.id_Msg == PAQ_FIN_MIGRAR); }
char IS_PAQ_ARCHIVO ( tPaqueteArch *paq ) { return (paq->id.id_Msg == PAQ_ARCHIVO); }

/*******************************************************************/
char* paquetes_PaqArchToChar( tPaqueteArch* paq  )
{
	memset( g_PaqArch, ' ', PAQUETE_ARCH_MAX_TAM );
	
	memcpy( &g_PaqArch, paq->id.IP, sizeof( paq->id.IP ) );
	memcpy( &(g_PaqArch[ POS_PAQ_ID_PORT ]), &paq->id.puerto, sizeof( paq->id.puerto ) );
	memcpy( &(g_PaqArch[ POS_PAQ_ID_PROC ]), &paq->id.id_Proceso, sizeof( paq->id.id_Proceso ) );
	memcpy( &(g_PaqArch[ POS_PAQ_ID_MSG ]), &paq->id.id_Msg, sizeof( paq->id.id_Msg ) );
	memcpy( &(g_PaqArch[ POS_PAQ_ID_ID_UU ]), &paq->id.UnUsed, sizeof( paq->id.UnUsed ) );
	
	memcpy( &(g_PaqArch[ POS_PAQ_LEN_MSG ]), &paq->msg_len, sizeof( paq->msg_len ) );
	memcpy( &(g_PaqArch[ POS_PAQ_MSG ]), &paq->msg, sizeof( paq->msg ) );

	return g_PaqArch;	
}

/*******************************************************************/
void paquetes_Archdestruir( tPaqueteArch* paq  )
{
	if ( paq )
		free( paq );
}

/*-----------------------------------------------------------------------------------------------------------*/
tPaquete* paquetes_newPaqMigrar( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, long lPCB_ID )
{
	tPaquete* paq;
	
	if ( (paq = CrearPaqSinBody( IP, id_Proceso,puerto, PAQ_MIGRAR ) ) )
	{ 
		memcpy( &paq->msg, &lPCB_ID, sizeof( long ) ); 
	}
	
	return paq;		
}

/*-----------------------------------------------------------------------------------------------------------*/
tPaquete* paquetes_newPaqMigrarOK( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	return CrearPaqSinBody( IP, id_Proceso,puerto, PAQ_MIGRAR_OK );		
}


/*-----------------------------------------------------------------------------------------------------------*/
tPaquete* paquetes_newPaqMigrarFault( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	return CrearPaqSinBody( IP, id_Proceso,puerto, PAQ_MIGRAR_FAULT );
}

/*-----------------------------------------------------------------------------------------------------------*/
tPaqueteArch* paquetes_newPaqFinMigrar( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaqueteArch *paq;
	
	if ( !(paq = paquetes_CrearArch() ) )
		return NULL;

	paquetes_CargarIdMSgArch( paq, IP, id_Proceso, PAQ_FIN_MIGRAR, puerto );
	
	return paq;
}

/*-----------------------------------------------------------------------------------------------------------*/
tPaqueteArch* paquetes_newArchivo( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, 
								char szArch[ MAX_PAQ_ARCH ] )
{
	tPaqueteArch *paq;
	
	if ( !(paq = paquetes_CrearArch() ) )
		return NULL;

	paquetes_CargarIdMSgArch( paq, IP, id_Proceso, PAQ_ARCHIVO, puerto );
	strncpy( paq->msg, szArch, MAX_PAQ_ARCH );
	
	return paq;		
}								

/*-----------------------------------------------------------------------------------------------------------*/
char* paquetes_newPaqMigrarAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, long lPCB_ID )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqMigrar(IP,id_Proceso, puerto, lPCB_ID )) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}

/*-----------------------------------------------------------------------------------------------------------*/
char* paquetes_newPaqMigrarOKAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqMigrarOK(IP,id_Proceso, puerto )) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}

/*-----------------------------------------------------------------------------------------------------------*/
char* paquetes_newPaqMigrarFaultAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqMigrarFault(IP,id_Proceso, puerto )) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}

/*-----------------------------------------------------------------------------------------------------------*/
char* paquetes_newPaqFinMigrarAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto )
{
	tPaqueteArch *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqFinMigrar(IP,id_Proceso, puerto )) )
	{
		Ret = paquetes_PaqArchToChar( pNew );
		
		paquetes_Archdestruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}

/*-----------------------------------------------------------------------------------------------------------*/
char* paquetes_newArchivoAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, 
								char szArch[ MAX_PAQ_ARCH ] )
{
	tPaqueteArch *pNew; char *Ret;
	if ( (pNew = paquetes_newArchivo(IP,id_Proceso, puerto, szArch )) )
	{
		Ret = paquetes_PaqArchToChar( pNew );
		
		paquetes_Archdestruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}

/*******************************************************************/
tPaqueteArch* paquetes_CharToPaqArch( const char* buffer  )
{
	tPaqueteArch *paq;
	char szAux[ PAQUETE_ARCH_MAX_TAM ];
	
	if ( !(paq = paquetes_CrearArch() ) )
		return NULL;
	
	memcpy( &paq->id.IP, &(buffer[ POS_PAQ_IP ]), sizeof( paq->id.IP ) );
	memcpy( &paq->id.puerto, &(buffer[ POS_PAQ_ID_PORT ]), sizeof( paq->id.puerto ) );
	memcpy( &paq->id.id_Proceso, &(buffer[ POS_PAQ_ID_PROC ]), sizeof( paq->id.id_Proceso ) );
	memcpy( &paq->id.id_Msg,  &(buffer[ POS_PAQ_ID_MSG ]), sizeof( paq->id.id_Msg ) );
	memcpy( &paq->id.UnUsed, &(buffer[ POS_PAQ_ID_ID_UU ]), sizeof( paq->id.UnUsed ) );
	
	memcpy( &paq->msg_len, &(buffer[ POS_PAQ_LEN_MSG ]), sizeof( paq->msg_len ) );
	memcpy( &paq->msg, &(buffer[ POS_PAQ_MSG ]), sizeof( paq->msg ) );
	
	return paq;
}

/*******************	PAQ_MIGRATE		*******************/
char IS_PAQ_MIGRATE (tPaquete *paq) { return (paq->id.id_Msg == PAQ_MIGRATE); }

#define MT_POS_IP 		0 
#define MT_POS_PUERTO	16

tPaquete* paquetes_newPaqMigrate( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									unsigned char IPDestino[4], unsigned short int puertoDestino )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_MIGRATE, puerto );
	
	paq->msg_len = PAQ_LEN_MSGCTRL;
	
	memcpy( &(paq->msg[MT_POS_IP]), IPDestino, sizeof(IPDestino) ); 
	memcpy( &(paq->msg[MT_POS_PUERTO]), &puertoDestino, sizeof( puertoDestino ) );
	
	return paq;		
	
}

tPaquete* paquetes_ParsearPaqMigrate( const char* Buffer, unsigned char* IP, unsigned char* id_Proceso, 
											unsigned short int* puerto,
											unsigned char* IPDestino, unsigned short int* puertoDestino  )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_CharToPaq( Buffer ) ) )
		return NULL;
	
	memcpy( IP, &(paq->id.IP), sizeof(char[4]) );
	memcpy( id_Proceso, &(paq->id.id_Proceso), sizeof(unsigned char) );
	memcpy( puerto, &(paq->id.puerto), sizeof(unsigned short int) );
	memcpy( IPDestino, &(paq->msg[MT_POS_IP]), sizeof( unsigned char[4] ) ); 
	memcpy( puertoDestino, &(paq->msg[MT_POS_PUERTO]), sizeof( unsigned short int ) );
	
	return paq;		

}

char* paquetes_newPaqMigrateAsStr( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto,
									unsigned char IPDestino[4], unsigned short int puertoDestino )
{
	tPaquete *pNew; char *Ret;
	if ( (pNew = paquetes_newPaqMigrate(IP,id_Proceso, puerto, IPDestino, puertoDestino )) )
	{
		Ret = paquetes_PaqToChar( pNew );
		
		paquetes_destruir( pNew );
		
		return Ret;
	}	
	
	return NULL;
}
#undef MT_POS_IP 
#undef MT_POS_PUERTO

/*--------------------- Migracion -------------------------------*/
/*-----------------------------paq_kill----------------------------*/
tPaquete* paquetes_newPaqKill( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int pidVec[25] )
{
	tPaquete *paq;
	int i;
		
		if ( !(paq = paquetes_Crear() ) )
			return NULL;

		paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_KILL, puerto );
		/*for(i=0;i<25;i++){
			memset( &paq->msg[2*i], 0,sizeof(int));
			memcpy( &paq->msg[2*i], &pidVec[i], sizeof(int)); 
		}*/
		memcpy(&paq->msg,&pidVec,25*sizeof(int));
		paq->msg_len = 25*sizeof(int);
		
		return paq;
}
char IS_PAQ_KILL ( tPaquete *paq ) { return (paq->id.id_Msg == PAQ_KILL); }



/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
