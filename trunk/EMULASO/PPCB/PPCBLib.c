/*
 ============================================================================
 Name        : PPCB.c
 Author      : Trimboli, Damián
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : App de PPCB
 ============================================================================
 */
 
 #include "PPCBLib.h"


 /*Variables privadas*/

sigset_t 		conjunto_seniales;
long 			g_lTime1 = 0;
long 			g_lTime2 = 0;
long			g_lpcb_id = 0;

/**************************************************\
 *            PROTOTIPOS DE FUNCIONES Privadas     *
\**************************************************/

/*************************************************************************/



/**********************************************************/
void PCB_ProcesarSeniales( int senial )
/*Advertencia al programador debbugueador: si pones un breakpoint aca
 * te va a romper a cada segundo. ;)*/
{
	int nstateChld;
	if ( senial == SIGALRM ) /*Timer*/
	{
		
	}
	else if ( senial == SIGUSR1 )
	{
	}
	else if ( senial == SIGUSR2 )
	{
	}
	else if ( senial == SIGHUP )
	{
	}
	else if ( senial == SIGINT )
	{			
		PCB_Salir();
	}
	else if ( senial == SIGCHLD )
	{	
		Log_log( log_warning, "Recibo senial SIGCHILD");
		wait( &nstateChld );
		Log_log( log_warning, "Certifico que murio un proc hijo(ADT)");
		signal( SIGCHLD, PCB_ProcesarSeniales );
	}
}


/**********************************************************************/
int PCB_Init(int argc, char *argv[] )
{
	do
	{
		Log_Inicializar( _PPCB_, "1" );

		Log_log( log_debug, "Inicio de Aplicacion" );
		
		if ( argc == 2) {
			
			PCB.PPCB_ID = atol (argv[1]);
			
			if ( PCB_LeerConfig() == ERROR )
			{
				Log_log( log_error, "Error leyendo la configuracion" ); 
				break;
			}
					
		} else if ( argc == 8) {
			
			
		}
		/* inicializacion de la lista de sockets */
		if ( !(PCB.m_ListaSockets = malloc( MALLOC_SOCKS_INI *sizeof(tSocket*) ) ) ) 
			return ERROR;
			
		memset( PCB.m_ListaSockets, 0, MALLOC_SOCKS_INI * sizeof(tSocket*) );
		
		/*Creo el socket de escucha*/
		if ( ! (PCB.m_ListaSockets[ SOCK_ESCUCHA ] = conexiones_crearSockEscucha( &PCB.m_Port, 10,  
														&PCB_AceptarConexion )) )
			break;
		
			
		PCB.m_ultimoSocket = SOCK_ESCUCHA;
		
		if ( PCB_ConectarConACR() == ERROR )
		{
			Log_log( log_error, "No se pudo establecer conexion con el ACR" );
			return ERROR;
		}
		
		signal(SIGALRM, PCB_ProcesarSeniales);
		signal(SIGCHLD, PCB_ProcesarSeniales);
	
		return OK;
		
	}while(0);
	
	if ( errno )
	{
		Log_log( log_error, conn_getLastError() );
	}
	else
	{
		printf("No se puede crear el Buscador: revise el archivo de configuracion\n");
	}
		
	exit(EXIT_FAILURE);
	
	return ERROR;
}

/**********************************************************************/
int PCB_LeerConfig()
{
	char    strConfig[20];
	char 	*tmp;
	int 	line=1, nStack=1, intValue;
	char 	strCode[10], strStack[10];
	tConfig *cfg;
	
	do
	{		
		sprintf(strConfig, "config.ppcb%s", PCB.PPCB_ID);
		
		if ( !(cfg = config_Crear( PCB.PPCB_ID, _PPCB_ )) ) 
			break;
		
		
		
		if ( (tmp = config_GetVal( cfg, _PPCB_, "CREATORID" ) ) )
		{
			strncpy( PCB.User, tmp, LEN_USUARIO );
		}
		
		sprintf(strCode, "CODE%d", line);
		
		while ( (tmp = config_GetVal( cfg, _PPCB_, strCode) ) ) {
			strncpy( PCB.Code[line-1], tmp, 20);
			
			line++;
			sprintf(strCode, "CODE%d", line);
			
		}
		
		if ( (PCB.IP = config_GetVal_Int( cfg, _PPCB_, "IPOINTER" ) ) ){
					
		}
		
		if (strcmp("PENDIENTE", config_GetVal( cfg, _PPCB_, "ESTADO")) ){
			PCB.State = PENDIENTE;
		} else if (strcmp("BLOQUEADO", config_GetVal( cfg, _PPCB_, "ESTADO")) ) {
			PCB.State = BLOQUEADO;
		} else if (strcmp("LISTO", config_GetVal( cfg, _PPCB_, "ESTADO")) ) {
			PCB.State = LISTO;
		} else if (strcmp("EJECUTANDO", config_GetVal( cfg, _PPCB_, "ESTADO")) ) {
			PCB.State = EJECUTANDO;
		} 
		
		init(&PCB.stack);
		
		sprintf(strStack, "STACK%d", nStack);
			
		while ( (intValue = config_GetVal_Int( cfg, _PPCB_, strStack) ) ) {
			push( &PCB.stack, intValue );
				
			nStack++;
			sprintf(strStack, "STACK%d", nStack);
		
		}
		
				
		PCB.m_ACR_Port = config_GetVal_Int( cfg, _PPCB_, "PUERTO_ACR" );
		
		/*Levanto la configuracion*/
		/*if ( (tmp = config_GetVal( cfg, _MSHELL_, "PCB_IP" ) ) )
		{
			strncpy( PCB.m_IP, tmp, LEN_IP );
		}
		*/
		
		if ( (tmp = config_GetVal( cfg, _PPCB_ , "IP_ACR" ) ) )
		{
			strncpy( PCB.m_ACR_IP, tmp, LEN_IP );
		}
	
		/*
		
		PCB.m_Port = config_GetVal_Int( cfg, _MSHELL_, "PCB_PORT" );
		*/
		/*PCB.m_nMemDisp = PCB.m_nMemMax = config_GetVal_Int( cfg, _PCB_, "MAX_MEM" );
		PCB.m_Q = config_GetVal_Int( cfg, _PCB_, "Q" );
		PCB.m_nLimite1 = config_GetVal_Int( cfg, _PCB_, "L1" );
		PCB.m_nLimite2 = config_GetVal_Int( cfg, _PCB_, "L2" );
		*/
		config_Destroy(cfg);

		return OK;
		
	}while(0);
	
	return ERROR;
}

/**********************************************************************/
int PCB_ConectarConACR()
{
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	ReducirIP( PCB.m_IP, szIP );
	
	if ( !( pSocket = conexiones_ConectarHost( PCB.m_ACR_IP, PCB.m_ACR_Port,
										 &PCB_ConfirmarConexion ) ) )
		return ERROR;
	
	PCB.m_ListaSockets[ SOCK_ACR ] = pSocket;
	PCB.m_ultimoSocket = SOCK_ACR;
	
	/*Mando el Ping al ACR*/
	Log_log( log_debug, "envio Ping para conectarme con ACR" );
	
	if ( !(pPaq  = paquetes_newPaqPing( szIP, _ID_PPCB_, conexiones_getPuertoLocalDeSocket(pSocket) )) )
		return ERROR;
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	
	paquetes_destruir( pPaq );
	
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
	
	return ERROR;
}

/**********************************************************/
void PCB_ConfirmarConexion( tSocket* sockIn )
{
	char* 			tmp; 
	int 			len; 
	char 			buffer[ PAQUETE_MAX_TAM ];
	tPaquete* 		paq = NULL;
	
	Log_printf( log_debug, "%s: %d confirma conexion!", 
			conexiones_getIpRemotaDeSocket( sockIn ), 
			(int) conexiones_getPuertoRemotoDeSocket( sockIn ) );
	
		
	len = conexiones_recvBuff( sockIn, buffer, PAQUETE_MAX_TAM );
	
	if ( ERROR == len || !len)
	{
		conexiones_CerrarSocket( PCB.m_ListaSockets, sockIn, &PCB.m_ultimoSocket );
		return;
	}
	
		
	paq = paquetes_CharToPaq( buffer );

	if ( IS_ACR_PAQ( paq ) &&  IS_PAQ_PONG ( paq ) )
	{/*Si el ACR me responde pong la conexion queda establecida!*/
		Log_log( log_debug, "Conexion establecida con el ACR!" );
		sockIn->callback = &PCB_AtenderACR;
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
}

/**********************************************************/
void PCB_AceptarConexion( tSocket* sockIn )
/* */
{
	char szLog[50];
	
	PCB.m_ListaSockets = realloc( PCB.m_ListaSockets, 
								( ++PCB.m_ultimoSocket + MALLOC_SOCKS_INI ) * sizeof( tSocket* ) );
							
	PCB.m_ListaSockets[ PCB.m_ultimoSocket ] = conexiones_crearSockAceptado( sockIn, 
																&PCB_HandShake );
	
	if ( !PCB.m_ListaSockets[ PCB.m_ultimoSocket ] )
	{
		sprintf( szLog, "Rechazada conexion de %s:%d", conexiones_getIpRemotaDeSocket( sockIn ), 
								(int) conexiones_getPuertoRemotoDeSocket( sockIn ) );
		Log_log(log_debug, szLog  );
		
		--( PCB.m_ultimoSocket );
	}
	else
	{
		sockIn->estado 			= estadoConn_escuchando;
		
		sprintf( szLog , "Aceptada conexion de %s:%d", 
			conexiones_getIpRemotaDeSocket( PCB.m_ListaSockets[ PCB.m_ultimoSocket ] ), 
			(int)conexiones_getPuertoRemotoDeSocket( PCB.m_ListaSockets[ PCB.m_ultimoSocket ] ) );
		
		Log_log( log_debug, szLog );
	}
}

/**********************************************************/
void PCB_HandShake( tSocket* sockIn )
/* Despues que se acepta la conexion, esta funcion es la mesa de entrada en donde
 * se identifica el proceso remitente, en base a este se le asigna un handler especifico*/
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ]; 
	unsigned char szIP[4];
	int nSend;
	
	memset( szIP, 0, 4 );


	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		PCB_CerrarConexion( sockIn );		
	
		return;
	}
	
	paq = paquetes_CharToPaq( buffer );

	if ( IS_PPCB_PAQ( paq ) && IS_PAQ_PING ( paq ) )
	{/*Bienvenido PCB!*/
		
		/*Log_log( log_debug, "PCB me manda un ping para establecer conexion" );
		
		sockIn->callback = &PCB_AtenderPCB;
		*/
		/*Mando el Pong al PCB*/
		/*paquetes_destruir( paq );
		paq = NULL; /*Se puede hacer una funcion en paquetesGeneral que devuelve un char directamente*/
		/*
		if ( !(paq  = paquetes_newPaqPong( szIP, _ID_PCB_, conexiones_getPuertoLocalDeSocket(sockIn) )) )
		{
			Log_log( log_error, "Error enviando pong al MShell" );
		}
			
		Log_log( log_debug, "Le respondo con un pong" );
		nSend = conexiones_sendBuff( sockIn, (const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
		
		if ( nSend != PAQUETE_MAX_TAM )
			Log_logLastError( "error enviando pong " );
		*/
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
}

/**********************************************************/
void PCB_AtenderACR ( tSocket *sockIn )
/**/
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ]; 
	tIDMensaje		idMsj;
	float 			fCargaProm = 0;
	int 			CantPCB = 0;
	char*			buffPaq;
	int				nSend;
	unsigned char	szIP[4] = {'\0','\0','\0','\0'};


	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		Log_log( log_debug, "Se cae conexion con ACR" );
		PCB_CerrarConexion( sockIn );		
	
		return;
	}
	
	paq = paquetes_CharToPaq(buffer);


	if ( IS_PAQ_GET_PERFORMANCE( paq ) )
	{/*Me pide la performance*/
		Log_log( log_debug, "el ACR me manda un GetPerformance" );

		
	}



	
	if ( paq ) 
		paquetes_destruir( paq );
	
}



/**********************************************************/
void PCB_Salir()
{
	/* Logear salida, hacer un clean up? */
	
	conexiones_CerrarLista( 0, &PCB.m_ultimoSocket, PCB.m_ListaSockets );
	
	Log_log(log_info,"Fin de la ejecucion");
	pantalla_Clear();
	exit(EXIT_SUCCESS);
}

/**********************************************************/
void 	PCB_CerrarConexion( tSocket *sockIn )
{/*Cerrar el socket correspondiente y tomar la accion correspondiente*/
}

/**********************************************************/


/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
