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


/* Protos privado */
tPaquete* paquetes_Crear();
void paquetes_CargarIdMSg( tPaquete *paq, unsigned char IP[4], unsigned char id_Proceso, unsigned char id_Msg,
								unsigned short int puerto );

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
	memcpy( paq->id.IP, IP, 4 );
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
	
	memcpy( &paq->id.IP, &buffer, sizeof( paq->id.IP ) );
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
tPaquete* paquetes_newPaqADSLogout( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, int idConeccion )
{
	tPaquete *paq;
		
		if ( !(paq = paquetes_Crear() ) )
			return NULL;

		paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_LOGOUT, puerto );
		memcpy( paq->msg, &idConeccion, sizeof(idConeccion));
		paq->msg_len = sizeof(idConeccion);
		
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
tPaquete* paquetes_newPaqPrint( unsigned char IP[4], unsigned char id_Proceso, unsigned short int puerto, char msg[30] )
{
	tPaquete *paq;
	
	if ( !(paq = paquetes_Crear() ) )
		return NULL;

	paquetes_CargarIdMSg( paq, IP, id_Proceso, PAQ_EXEC_PROG, puerto );
	paquetes_CargarMSg( paq, msg );
	
	return paq;		
}
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
