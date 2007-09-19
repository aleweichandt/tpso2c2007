/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: ADSLib.c
 * 
 * Historia
 *	10-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */
 
 #include "ADSLib.h"

 /*Variables privadas*/
sigset_t 		conjunto_seniales;


/**************************************************\
 *            PROTOTIPOS DE FUNCIONES Privadas     *
\**************************************************/


/**********************************************************/
void ADS_ProcesarSeniales( int senial )
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
		ADS_Salir();
	}
	else if ( senial == SIGCHLD )
	{	
		Log_log( log_warning, "Recibo senial SIGCHILD");
		wait( &nstateChld );
		Log_log( log_warning, "Certifico que murio un proc hijo(ADT)");
		signal( SIGCHLD, ADS_ProcesarSeniales );
	}
}


/**********************************************************************/
int ADS_Init( )
{
	do
	{
		Log_Inicializar( _ADS_, "1" );
		
		Log_log( log_debug, "Inicio de Aplicacion" );
		
				
		if ( ADS_LeerConfig() == ERROR )
		{
			Log_log( log_error, "Error leyendo la configuracion" ); 
			break;
		}
				
		/* inicializacion de la lista de sockets */
		if ( !(ADS.m_ListaSockets = malloc( MALLOC_SOCKS_INI *sizeof(tSocket*) ) ) ) 
			return ERROR;
			
		memset( ADS.m_ListaSockets, 0, MALLOC_SOCKS_INI * sizeof(tSocket*) );
		
		/*Creo el socket para el teclado*/
		if ( ! (ADS.m_ListaSockets[ SOCK_ESCUCHA ] = conexiones_crearSockEscucha( &ADS.m_Port, 10,  
														&ADS_AceptarConexion )) )
			break;
			
		ADS.m_ultimoSocket = SOCK_ESCUCHA;
		
		signal(SIGALRM, ADS_ProcesarSeniales);
		signal(SIGCHLD, ADS_ProcesarSeniales);
	
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
int ADS_LeerConfig()
{
	char 	*tmp;
	tConfig *cfg;
	
	do
	{		
		if ( !(cfg = config_Crear( "config", _ADS_ )) ) 
			break;
		
		/*Levanto la configuracion*/
		if ( (tmp = config_GetVal( cfg, _ADS_, "ACR_IP" ) ) )
		{
			strncpy( ADS.m_ACR_IP, tmp, LEN_IP );
		}
		
		ADS.m_ACR_Port = config_GetVal_Int( cfg, _ADS_, "ACR_PORT" );
		
		/*Levanto la configuracion*/
		if ( (tmp = config_GetVal( cfg, _MSHELL_, "ADS_IP" ) ) )
		{
			strncpy( ADS.m_IP, tmp, LEN_IP );
		}
		
		ADS.m_Port = config_GetVal_Int( cfg, _MSHELL_, "ADS_PORT" );
		
			

		config_Destroy(cfg);

		return OK;
		
	}while(0);
	
	return ERROR;
}

/**********************************************************************/
int ACR_ConectarACR()
{
	return ERROR;
}

/**********************************************************/
void ADS_AceptarConexion( tSocket* sockIn )
/* */
{
	char szLog[50];
	
	ADS.m_ListaSockets = realloc( ADS.m_ListaSockets, 
								( ++ADS.m_ultimoSocket + MALLOC_SOCKS_INI ) * sizeof( tSocket* ) );
							
	ADS.m_ListaSockets[ ADS.m_ultimoSocket ] = conexiones_crearSockAceptado( sockIn, 
																&ADS_HandShake );
	
	if ( !ADS.m_ListaSockets[ ADS.m_ultimoSocket ] )
	{
		sprintf( szLog, "Rechazada conexion de %s:%d", conexiones_getIpRemotaDeSocket( sockIn ), 
								(int) conexiones_getPuertoRemotoDeSocket( sockIn ) );
		Log_log(log_debug, szLog  );
		
		--( ADS.m_ultimoSocket );
	}
	else
	{
		sockIn->estado 			= estadoConn_escuchando;
		
		sprintf( szLog , "Aceptada conexion de %s:%d", 
			conexiones_getIpRemotaDeSocket( ADS.m_ListaSockets[ ADS.m_ultimoSocket ] ), 
			(int)conexiones_getPuertoRemotoDeSocket( ADS.m_ListaSockets[ ADS.m_ultimoSocket ] ) );
		
		Log_log( log_debug, szLog );
	}
}

/**********************************************************/
void ADS_HandShake( tSocket* sockIn )
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
		ADS_CerrarConexion( sockIn );		
	
		return;
	}
	
	paq = paquetes_CharToPaq( buffer );

	if ( IS_MSHELL_PAQ( paq ) && IS_PAQ_PING ( paq ) )
	{/*Bienvenido MShell!*/
		
		Log_log( log_debug, "MShell me manda un ping para establecer conexion" );
		
		sockIn->callback = &ADS_AtenderMSH;
		
		/*Mando el Pong al ADS*/
		paquetes_destruir( paq );
		paq = NULL; /*Se puede hacer una funcion en paquetesGeneral que devuelve un char directamente*/
		
		if ( !(paq  = paquetes_newPaqPong( szIP, _ID_ADS_, conexiones_getPuertoLocalDeSocket(sockIn) )) )
		{
			Log_log( log_error, "Error enviando pong al MShell" );
		}
			
		Log_log( log_debug, "Le respondo con un pong" );
		nSend = conexiones_sendBuff( sockIn, (const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
		
		if ( nSend != PAQUETE_MAX_TAM )
			Log_logLastError( "error enviando pong " );
		
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
}

/**********************************************************/
void ADS_AtenderACR ( tSocket *sockIn )
/**/
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ 66 ]; 
	tIDMensaje		idMsj;


	len = conexiones_recvBuff(sockIn, buffer, 66);
	
	if ( ERROR == len || !len)
	{
		ADS_CerrarConexion( sockIn );		
	
		return;
	}
	
	paq = paquetes_CharToPaq(buffer);

	
	if ( paq ) 
		paquetes_destruir( paq );
	
}

/**********************************************************/
void ADS_AtenderMSH ( tSocket *sockIn )
/**/
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ 66 ]; 
	tIDMensaje		idMsj;


	len = conexiones_recvBuff(sockIn, buffer, 66);
	
	if ( ERROR == len || !len)
	{
		ADS_CerrarConexion( sockIn );		
	
		return;
	}
	
	paq = paquetes_CharToPaq(buffer);

	if ( IS_PAQ_USR_NAME( paq ) )
	{/*Me llega el nombre de usuario*/
	}
	else if ( IS_PAQ_USR_PWD( paq ) )
	{/*Me llega el password*/
	}
	
	
	if ( paq ) 
		paquetes_destruir( paq );
	
}


/**********************************************************/
void ADS_Salir()
{
	/* Logear salida, hacer un clean up? */
	
	conexiones_CerrarLista( 0, &ADS.m_ultimoSocket, ADS.m_ListaSockets );
	
	Log_log(log_info,"Fin de la ejecucion");
	pantalla_Clear();
	exit(EXIT_SUCCESS);
}

/**********************************************************/
void 	ADS_CerrarConexion( tSocket *sockIn )
{/*Cerrar el socket correspondiente y tomar la accion correspondiente*/
}
 
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
