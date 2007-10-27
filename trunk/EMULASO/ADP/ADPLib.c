/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: ADPLib.c
 * 
 * Historia
 *	13-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */
 
 #include "ADPLib.h"
 #include "DatosPCBs.c"
 /*#include "../incGeneral/Sockets/paquetesGeneral.c"*/
/*#include "../incGeneral/Sockets/paquetesADP.c"*/

 /*Variables privadas*/
sigset_t 		conjunto_seniales;
long 			g_lTime1 = 0;
long 			g_lTime2 = 0;
int				g_nAlarmaActiva = 1;

/**************************************************\
 *            PROTOTIPOS DE FUNCIONES Privadas     *
\**************************************************/

void TestPlanificacion()
{
	return;
	tunPCB	*ppcb; int i;
	
	ppcb = pcb_Crear( "127.0.0.1", "", 1, 9700, NULL, ADP.m_ListaSockets[SOCK_ACR], 5, 50, 9000 );
	lpcb_AgregarALista( &ADP.m_LPL, ppcb );
	
	ppcb = pcb_Crear( "127.0.0.1", "", 2, 9700, NULL, ADP.m_ListaSockets[SOCK_ACR], -5, 50, 9000 );
	lpcb_AgregarALista( &ADP.m_LPL, ppcb );

	ppcb = pcb_Crear( "127.0.0.1", "", 3, 9700, NULL, ADP.m_ListaSockets[SOCK_ACR], 5, 50, 9000 );
	lpcb_AgregarALista( &ADP.m_LPL, ppcb );

	ppcb = pcb_Crear( "127.0.0.1", "", 4, 9700, NULL, ADP.m_ListaSockets[SOCK_ACR], -5, 50, 9000 );
	lpcb_AgregarALista( &ADP.m_LPL, ppcb );
/*	
for ( i = 0; i < 3; i++ )
{	
	sleep( ADP.m_Q );
	
	ADP_Dispatcher( 1 );
}
*/
	
}

void InformarLista( tListaPCB Lista )
{
	tunPCB			*pPCB;
	
	while( Lista )
	{
		pPCB = lpcb_Datos( Lista );
		
		Log_printf( log_debug, "PCB %ld , Q %d", pPCB->id, pPCB->Q );			
		
		Lista = lpcb_Siguiente( Lista );
	}
}

/*------------------------------------------------------------------*/
void ADP_DesactivarAlarma()
{
	Log_printf( log_debug, "Se desactiva alarma" );
	g_nAlarmaActiva = 0;
/*	
	nRestoAlarma = alarm( 0 );
	Log_printf( log_debug, "Restaban %d seg para el Q", nRestoAlarma );
*/	
}

/*------------------------------------------------------------------*/
void ADP_ActivarAlarma()
{
	if ( g_nAlarmaActiva )
		return;
		
	Log_printf( log_debug, "Se Activa alarma" );
		
	if ( !ADP_SeCumplioQ() && !ADP_SeCumplioTimerAverage() )
	{
		alarm( 1 );
		signal(SIGALRM, &ADP_Dispatcher ); 
	}
	else
	{
		ADP_Dispatcher( SIGALRM );	
	}
	
	g_nAlarmaActiva = 1;
}



/*************************************************************************/
int ADP_SeCumplioQ()
{
	time_t ahoraGlobal = time( NULL );
	struct tm* ahoraLocal = localtime(&ahoraGlobal);
	long  lTime2;
	
	if ( !g_lTime1 )
		g_lTime1 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
	
	lTime2 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
	
	/*Log_printf( log_debug, "Time1 %ld Time2 %ld", g_lTime1, lTime2 );*/
	
	if ( lTime2 - g_lTime1 >= ADP.m_Q )
	{
		return 1;
	}
	
	return 0;
}

/*************************************************************************/
int ADP_SeCumplioTimerAverage()
{
	time_t ahoraGlobal = time( NULL );
	struct tm* ahoraLocal = localtime(&ahoraGlobal);
	long  lTime2;
	
	if ( !g_lTime2 )
		g_lTime2 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
	
	lTime2 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
	
	/*Log_printf( log_debug, "Time1 %ld Time2 %ld", g_lTime1, lTime2 );*/
	
	if ( lTime2 - g_lTime2 >= _TIMER_AVERAGE_ )
	{
		return 1;
	}
	
	return 0;
}


/*************************************************************************/
void ADP_Dispatcher(int n)
{
	time_t ahoraGlobal = time( NULL );
	struct tm* ahoraLocal = localtime(&ahoraGlobal);
	long  lTime1;
	int		nDif;
	
	if ( ADP_SeCumplioQ() )
	{
		Log_log( log_debug, "<< Entra al Dispatcher >>" );
		
		/* Esto se puede mejorar para que los saque exactamente a medida que se van venciendo
		 * con eso saca todos los que se hayan vencido.*/
		lTime1 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
		nDif = lTime1 - g_lTime1;
		lpcb_DecrementarQ( &ADP.m_LPE, nDif );
		/**/
		
		/*Le informo a todas las de LTP que paren*/
		ADP_InformarSuspencion();
	
		/*Las paso a LTL*/
		Log_printf( log_debug, "LPL" );
		InformarLista( ADP.m_LPL );
		
		Log_printf( log_debug, "LPE" );
		InformarLista( ADP.m_LPE );
		
		lpcb_PasarDeLTPaLTL( &ADP.m_LPE, &ADP.m_LPL, &ADP.m_nMemDisp ); 
	
		/*Ahora paso las primeras de LTL a LTP segun la capacidad de memoria*/
		ADP_PasarDeLPLaLPE();

		Log_printf( log_debug, "LPL" );
		InformarLista( ADP.m_LPL );
		
		Log_printf( log_debug, "LPP" );
		InformarLista( ADP.m_LPE );

		
		/*Les digo Start a todas las de LTP*/
		ADP_InformarReanudacion();
		
		g_lTime1 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
		
		Log_log( log_debug, "<< Sale del Dispatcher >>" );
	}
	
	if ( ADP_SeCumplioTimerAverage() )
	{
		if ( ADP_EstoyCargado() )
		{
			ADP_MigrarPCBPesado();
		}
		
		g_lTime2 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
	}
	
	/*Si entro aca quiere decir que se cumplio el timer asi que reseteo la senial*/
	alarm( 1 );
	signal( SIGALRM, &ADP_Dispatcher );
}

/************************************************************************************************/
int ADP_PasarDeLPLaLPE()
/*La multigramacion me la determina el limite de memoria*/
{
	tunPCB			*pPCB;
	tListaPCB		Lista = ADP.m_LPL;
	
	Log_printf( log_debug, "Voy a pasar de LPL a LPE. Mem. Disp. = %d", ADP.m_nMemDisp );
	
	while( Lista )
	{
		pPCB = lpcb_Datos( Lista );
		
		if ( !pPCB->pSocket )
		{
			Log_printf( log_debug, "El PCB %ld todavia no establecio conexion porque tiene el socket en null", 
									pPCB->id );
									
			Lista = lpcb_Siguiente( Lista );
			continue;			
		}
		
		
		if ( (ADP.m_nMemDisp - pPCB->MemoriaRequerida) >= 0 )
		{
			ADP.m_nMemDisp -= pPCB->MemoriaRequerida;

			Log_printf( log_debug, "Paso el PCB %ld, mem %d a LPE. Mem. Disp. = %d", pPCB->id, 
								pPCB->MemoriaRequerida, ADP.m_nMemDisp );			
			
			Lista = lpcb_Siguiente( Lista );/*no sacar por el ojo de abajo*/
			pPCB->Q = ADP.m_Q;
			lpcb_PasarDeLista( &ADP.m_LPL, &ADP.m_LPE, pPCB->id );/*ojo que aca dentro elimina el puntero para pasarlo a la otra*/
			continue;
		}
		
		Lista = lpcb_Siguiente( Lista );
	}
}

/************************************************************************************************/
int 	ADP_InformarSuspencion() 
{
	tunPCB			*pPCB;
	tListaPCB		Lista = ADP.m_LPE;
	unsigned char	szIPReducido[4];
				
	while( Lista )
	{
		pPCB = lpcb_Datos( Lista );
		ReducirIP( ADP.m_IP, szIPReducido );	
		
		if ( pPCB->Q <= 0 )
		{
			Log_printf( log_debug, "envio Suspension al pcb %ld", pPCB->id );
	
			if ( conexiones_sendBuff( pPCB->pSocket, (const char*) paquetes_newPaqSuspendPCBAsStr( szIPReducido, _ID_ADP_, ADP.m_Port ), 
					PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
			{
				Log_logLastError("enviando suspend_pcb");
			}
		}
				
		Lista = lpcb_Siguiente( Lista );
	}
	usleep(1000);
	  
	return 1;
}


/**********************************************************/
void ADP_InformarReanudacion()
{
	int 			len; 
	int 			nCont = 0;
	tunPCB			*pPCB;
	tListaPCB		Lista = ADP.m_LPE;
	unsigned char	szIPReducido[4];
					
	while( Lista )
	{
		pPCB = lpcb_Datos( Lista );
		
		Log_printf( log_debug,"Envio Start al PCB: %ld por fd = %d", 
						pPCB->id, pPCB->pSocket->descriptor );
		ReducirIP( ADP.m_IP, szIPReducido );	
		if ( conexiones_sendBuff( pPCB->pSocket, (const char*) paquetes_newPaqExecPCBAsStr( szIPReducido, _ID_ADP_, ADP.m_Port ), 
				PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
		{
			Log_logLastError("enviando exec_pcb");
		}
		/*Log_printf( log_debug,"Se envio un EXEC al PCB %d", pPCB->id);*/
		
		Lista = lpcb_Siguiente( Lista );
	}
}



/*TODO*/
int		ADP_EstoyCargado(){return 1;}
int		ADP_MigrarPCBPesado() {return 1;}
/*TODO*/

/**********************************************************/
void ADP_ProcesarSeniales( int senial )
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
	else if ( senial == SIGINT || senial == SIGTERM )
	{	
		Log_log( log_warning, "Recibo senial SIGTERM p SIGINT");		
		ADP_Salir();
	}
	else if ( senial == SIGCHLD )
	{	
		Log_log( log_warning, "Recibo senial SIGCHILD");
		wait( &nstateChld );
		if(  WIFEXITED(nstateChld) )
		{
			Log_printf( log_warning, "Murio un proc hijo en forma normal. ExitStatus: %d",
						WEXITSTATUS(nstateChld));
		}
		else
		{
			Log_log( log_warning, "Murio un proc hijo en forma Anormal");
		}
		signal( SIGCHLD, ADP_ProcesarSeniales );
	}
}


/**********************************************************************/
int ADP_Init( )
{
	time_t ahoraGlobal = time( NULL );
	struct tm* ahoraLocal = localtime(&ahoraGlobal);
	
	do
	{
		Log_Inicializar( _ADP_, "1" );

		Log_log( log_debug, "Inicio de Aplicacion" );
		
		ADP_CalcularCargaPromReal();		
		
		ADP.m_LPL = NULL;
		ADP.m_LPB = NULL;
		ADP.m_LPE = NULL;
		
						
		if ( ADP_LeerConfig() == ERROR )
		{
			Log_log( log_error, "Error leyendo la configuracion" ); 
			break;
		}
		
		ADP.m_nMemDisp = ADP.m_nMemMax;
				
		/* inicializacion de la lista de sockets */
		if ( !(ADP.m_ListaSockets = malloc( MALLOC_SOCKS_INI *sizeof(tSocket*) ) ) ) 
			return ERROR;
			
		memset( ADP.m_ListaSockets, 0, MALLOC_SOCKS_INI * sizeof(tSocket*) );
		
		/*Creo el socket de escucha*/
		if ( ! (ADP.m_ListaSockets[ SOCK_ESCUCHA ] = conexiones_crearSockEscucha( &ADP.m_Port, 10,  
														&ADP_AceptarConexion )) )
			break;
			
		ADP.m_ultimoSocket = SOCK_ESCUCHA;
		
		if ( ADP_ConectarConACR() == ERROR )
		{
			Log_log( log_error, "No se pudo establecer conexion con el ACR" );
			return ERROR;
		}
		
		signal(SIGALRM, ADP_ProcesarSeniales);
		signal(SIGCHLD, ADP_ProcesarSeniales);
		
		struct tm* ahoraLocal = localtime(&ahoraGlobal);
		signal(SIGTERM, ADP_ProcesarSeniales);
		signal(SIGINT, ADP_ProcesarSeniales);
	
	
		if ( !g_lTime1 )
			g_lTime1 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
		
		if ( !g_lTime2 )
			g_lTime2 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
		
		alarm( 1 );
		signal( SIGALRM, &ADP_Dispatcher );
	
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
int ADP_LeerConfig()
{
	char 	*tmp;
	tConfig *cfg;
	
	do
	{		
		if ( !(cfg = config_Crear( "config", _ADP_ )) ) 
			break;
		
		/*Levanto la configuracion*/
		if ( (tmp = config_GetVal( cfg, _ADP_, "ACR_IP" ) ) )
		{
			strncpy( ADP.m_ACR_IP, tmp, LEN_IP );
		}
		
		ADP.m_ACR_Port = config_GetVal_Int( cfg, _ADP_, "ACR_PORT" );
		
		/*Levanto la configuracion*/
		if ( (tmp = config_GetVal( cfg, _ADP_, "ADP_IP" ) ) )
		{
			strncpy( ADP.m_IP, tmp, LEN_IP );
		}
		
		ADP.m_Port = config_GetVal_Int( cfg, _ADP_, "ADP_PORT" );
		
		ADP.m_nMemDisp = ADP.m_nMemMax = config_GetVal_Int( cfg, _ADP_, "MAX_MEM" );
		ADP.m_Q = config_GetVal_Int( cfg, _ADP_, "Q" );
		ADP.m_nLimite1 = config_GetVal_Int( cfg, _ADP_, "L1" );
		ADP.m_nLimite2 = config_GetVal_Int( cfg, _ADP_, "L2" );

		config_Destroy(cfg);

		return OK;
		
	}while(0);
	
	return ERROR;
}

/**********************************************************************/
int ADP_ConectarConACR()
{
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	ReducirIP( ADP.m_IP, szIP );
	
	if ( !( pSocket = conexiones_ConectarHost( ADP.m_ACR_IP, ADP.m_ACR_Port,
										 &ADP_ConfirmarConexion ) ) )
		return ERROR;
	
	ADP.m_ListaSockets[ SOCK_ACR ] = pSocket;
	ADP.m_ultimoSocket = SOCK_ACR;
	
	/*Mando el Ping al ACR*/
	Log_log( log_debug, "envio Ping para conectarme con ACR" );
	
	if ( !(pPaq  = paquetes_newPaqPing( szIP, _ID_ADP_, conexiones_getPuertoLocalDeSocket(pSocket) )) )
		return ERROR;
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	
	paquetes_destruir( pPaq );
	
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
	
	return ERROR;
}

/**********************************************************/
void ADP_ConfirmarConexion( tSocket* sockIn )
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
		conexiones_CerrarSocket( ADP.m_ListaSockets, sockIn, &ADP.m_ultimoSocket );
		return;
	}
	
		
	paq = paquetes_CharToPaq( buffer );

	if ( IS_ACR_PAQ( paq ) &&  IS_PAQ_PONG ( paq ) )
	{/*Si el ACR me responde pong la conexion queda establecida!*/
		Log_log( log_debug, "Conexion establecida con el ACR!" );
		sockIn->callback = &ADP_AtenderACR;
		
		TestPlanificacion();
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
}

/**********************************************************/
void ADP_AceptarConexion( tSocket* sockIn )
/* */
{
	char szLog[50];
	
	ADP.m_ListaSockets = realloc( ADP.m_ListaSockets, 
								( ++ADP.m_ultimoSocket + MALLOC_SOCKS_INI ) * sizeof( tSocket* ) );
							
	ADP.m_ListaSockets[ ADP.m_ultimoSocket ] = conexiones_crearSockAceptado( sockIn, 
																&ADP_HandShake );
	
	if ( !ADP.m_ListaSockets[ ADP.m_ultimoSocket ] )
	{
		sprintf( szLog, "Rechazada conexion de %s:%d", conexiones_getIpRemotaDeSocket( sockIn ), 
								(int) conexiones_getPuertoRemotoDeSocket( sockIn ) );
		Log_log(log_debug, szLog  );
		
		--( ADP.m_ultimoSocket );
	}
	else
	{
		sockIn->estado 			= estadoConn_escuchando;
		
		sprintf( szLog , "Aceptada conexion de %s:%d", 
			conexiones_getIpRemotaDeSocket( ADP.m_ListaSockets[ ADP.m_ultimoSocket ] ), 
			(int)conexiones_getPuertoRemotoDeSocket( ADP.m_ListaSockets[ ADP.m_ultimoSocket ] ) );
		
		Log_log( log_debug, szLog );
	}
}

/**********************************************************/
void ADP_HandShake( tSocket* sockIn )
/* Despues que se acepta la conexion, esta funcion es la mesa de entrada en donde
 * se identifica el proceso remitente, en base a este se le asigna un handler especifico*/
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ]; 
	unsigned char szIP[4];
	int nSend;
	long			lpcb_id;
	tunPCB			*pcb;
	
	memset( szIP, 0, 4 );


	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		ADP_CerrarConexion( sockIn );		
	
		return;
	}
	
	paq = paquetes_CharToPaq( buffer );

	if ( IS_PPCB_PAQ( paq ) && IS_PAQ_PING ( paq ) )
	{/*Bienvenido PCB!*/
		
		memcpy( &lpcb_id, paq->msg, sizeof(long) );
		
		if ( (pcb = lpcb_BuscarPCBxid( &ADP.m_LPL, lpcb_id )) ) /*Busco el pcb y actualizo el socket*/
		{/*Tiene que estar en la de LPL*/
			pcb->pSocket = sockIn;
		}
		
		Log_log( log_debug, "PCB me manda un ping para establecer conexion" );
		
		sockIn->callback = &ADP_AtenderPCB;
		
		/*Mando el Pong al ADP*/
		paquetes_destruir( paq );
		paq = NULL; /*Se puede hacer una funcion en paquetesGeneral que devuelve un char directamente*/
		
		if ( !(paq  = paquetes_newPaqPong( szIP, _ID_ADP_, conexiones_getPuertoLocalDeSocket(sockIn) )) )
		{
			Log_log( log_error, "Error enviando pong al PPCB" );
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
void ADP_AtenderACR ( tSocket *sockIn )
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
		ADP_CerrarConexion( sockIn );		
	
		return;
	}
	
	paq = paquetes_CharToPaq(buffer);


	if ( IS_PAQ_GET_PERFORMANCE( paq ) )
	{/*Me pide la performance*/
		Log_log( log_debug, "el ACR me manda un GetPerformance" );

		if ( (fCargaProm = ADP_CalcularCargaPromReal()) <= 0.0 )
			fCargaProm = 1.0;
		
		CantPCB = (float)ADP.m_nMemMax / fCargaProm; 
		Log_printf(log_debug,"CantPCB(%d) = ADP.m_nMemMax(%.5f) / fCargaProm(%.5f) = (%.5f)",
					CantPCB,(float)ADP.m_nMemMax, fCargaProm, (float)ADP.m_nMemMax / fCargaProm);
		ReducirIP( ADP.m_IP, szIP ); /*01-10-07:LAS: Esto lo tenia que reducir*/
		Log_printf(log_debug,"ip:%s,puerto=%d",ADP.m_IP,ADP.m_Port);
		if( (buffPaq = paquetes_newPaqInfoPerformanceAsStr( szIP, _ID_ADP_, ADP.m_Port,
											ADP.m_nMemMax, fCargaProm, CantPCB )) )
		{
			Log_log( log_debug, "Envio un InfoPerformance" );
			nSend = conexiones_sendBuff( sockIn, buffPaq, PAQUETE_MAX_TAM );
		}
	}
	/* atendewr end_sesion en adp */
	else if( IS_PAQ_KILL(paq) )
	{
		int pidVector[25];
		int i;
		char p[50];
		
		Log_log( log_debug, "el ACR me manda un kill por end_sesion" );
		
		memcpy(p,&paq->msg,50);
		
		for(i=0;i<25;i++){
			pidVector[i]=(p[2*i]*256)+p[2*i+1];
			/*memcpy( &pidVector[i], &paq->msg[2*i], sizeof(int));*/ 
		}
		i=0;
		/*memcpy(pidVector,&paq->msg,25*sizeof(int));*/
		
		/* se eliminan los pcbs de la lista si tienen igual id */
		/*for (i=0; i<25; i++){*/
		while(pidVector[i]!=0){
				/* aca se elimina de la lista */
			ADP_LiberarRecursos(pidVector[i]);
			i++;
		}
		
	}



	
	if ( paq ) 
		paquetes_destruir( paq );
	
}

/**********************************************************/
void ADP_AtenderPCB ( tSocket *sockIn )
/**/
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ];
	FILE*			arch;
	char			szPathArch[512]; 
	long			lpcb_id;


	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		Log_logLastError("Se cierra socket al atender PCB");
		ADP_CerrarConexion( sockIn );		
	
		return;
	}
	else if( len != PAQUETE_MAX_TAM )
	{ 
		Log_printf( log_warning, "Atencion no recibi el largo fijo! de paquete(%d/%d)",
					len, PAQUETE_MAX_TAM ); 
	}
	
	paq = paquetes_CharToPaq(buffer);

	if ( IS_PAQ_MIGRAR( paq ) )
	{/*Me llega el paquete migrar, cambio el handshake para recibir el archivo del pcb data*/
		Log_log( log_debug, "Llega un Migrar..." );
		
		ADP_DesactivarAlarma();
		
		memcpy( &lpcb_id, paq->msg, sizeof(long) );
		bzero(szPathArch,sizeof(szPathArch));
		
		Log_printf( log_debug, "...del pcb %ld", lpcb_id );
		
		ArmarPathPCBConfig( szPathArch, lpcb_id );
		
		
		if ( (arch = fopen( szPathArch, "wb+" )) )
		{
			sockIn->callback = &ADP_RecibirArchivo;
			sockIn->extra = (void*)lpcb_id;
			fclose(arch);
			Log_printf(log_debug,"Se creo el archivo %s",szPathArch);
		}
		else
		{
			Log_logLastError( "No pude abrir el archivo de la migracion" );
		}
	}
	else if(IS_PAQ_PRINT(paq))
	{/*me llega el print, se lo reenvio al ACR*/
		int nSend;
		
		Log_log( log_debug, "Mando PRINT al ACR" );
		nSend = conexiones_sendBuff( ADP.m_ListaSockets[SOCK_ACR], (const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
					
		if(nSend != PAQUETE_MAX_TAM)
		{
			Log_logLastError( "error al enviar PRINT al ACR" );
		}
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
	
}

/***********************************************************/
void ADP_RecibirArchivo( tSocket *sockIn )
{
	int 			len; 
	tPaqueteArch* 	paq ; 
	tPaquete* 		paq2 ;
	FILE*			arch;
	char 			*tmp;
	char			buffer [ PAQUETE_ARCH_MAX_TAM ];
	char			szPathArch[512];	 
	unsigned char	szIP[4];
	long			lpcb_id;
	int				nMemoria;
	long			lpid;

	Log_printf(log_debug,"Entro en ADP_RecibirArchivo");

	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_ARCH_MAX_TAM );
	
	if ( ERROR == len || !len)
	{
		Log_logLastError("Recibiendo datos de socket en ADP_RecibirArchivo");
		ADP_CerrarConexion( sockIn );
		
		ADP_ActivarAlarma();		
	
		return;
	}
	
	
	if( len != PAQUETE_ARCH_MAX_TAM )
	{ 
		Log_printf(log_warning,
			"Atencion no recibi el largo fijo de paquete. Me llega %d en vez de %d al transferir",
			len, PAQUETE_ARCH_MAX_TAM ); 
	}
	
	if( !(paq = paquetes_CharToPaqArch(buffer)) ){
		Log_logLastError("Al crear el paquete en ADP_RecibirArchivo");
	}

	if ( IS_PAQ_ARCHIVO( paq ) )
	{/*Llega el paq archivo -> persisto el contenido*/
		Log_log( log_debug, "Llega un PAQ_ARCHIVO" );
		
		bzero(szPathArch,sizeof(szPathArch));
		lpcb_id = (long)sockIn->extra;
		ArmarPathPCBConfig( szPathArch, lpcb_id );
		
		if ( (arch = fopen( szPathArch, "ab+" )) ){
			fprintf( arch, "%s", paq->msg );
			fclose(arch);
		}else{
			Log_logLastError( "No pude abrir el archivo de la migracion" );

			/*Envio el msj de migracion Fault*/
			Log_log( log_debug, "envio migrar Fault" );
			ReducirIP( ADP.m_IP, szIP);
			if ( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqMigrarFault( szIP, _ID_ADP_, ADP.m_Port ), 
					PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
			{
				Log_logLastError("enviando migrar_Fault");
			}
		}
	}
	else if ( IS_PAQ_FIN_MIGRAR( paq ) )
	{
		Log_log( log_debug, "llega un PAQ_FIN_MIGRAR" );
		lpcb_id = (long)sockIn->extra;
		sockIn->callback = 	&ADP_AtenderPCB;
		
		/*Envio el msj de migracion OK*/
		Log_log( log_debug, "envio migrar ok" );
		ReducirIP( ADP.m_IP, szIP);
		if ( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqMigrarOKAsStr( szIP, _ID_ADP_, ADP.m_Port ), 
				PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
		{
			Log_logLastError("enviando migrar_ok");
		}
		else
		{
			if ( ADP_ForkearPCB( lpcb_id, &lpid ) == OK )
			{
				memcpy( &nMemoria, &(paq->id.UnUsed[0]), sizeof(int) );
				ADP_CrearPCB( lpcb_id, sockIn, nMemoria, lpid );/*Lo crea y lo agrega a la lista de listos*/
			}
			else
			{
				/*Envio el msj de migracion Fault*/
				Log_log( log_debug, "envio migrar Fault" );
				ReducirIP( ADP.m_IP, szIP);
				if ( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqMigrarFault( szIP, _ID_ADP_, ADP.m_Port ), 
						PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
				{
					Log_logLastError("enviando migrar_Fault");
				}
			}
		}
		
		ADP_DesactivarAlarma();
	}
	
	if ( paq ) 
		paquetes_Archdestruir( paq );
	
}

/**********************************************************/
void ADP_Salir()
{
	/* Logear salida, hacer un clean up? */
	
	conexiones_CerrarLista( 0, &ADP.m_ultimoSocket, ADP.m_ListaSockets );
/*
	Log_log(log_info,"matando los pcbs de LPL");
	lpcb_MatarPCBs( &ADP.m_LPL );
	Log_log(log_info,"matando los pcbs de LPE");
	lpcb_MatarPCBs( &ADP.m_LPE );
	Log_log(log_info,"matando los pcbs de LPB");
	lpcb_MatarPCBs( &ADP.m_LPB );
*/
	Log_log(log_info,"Fin de la ejecucion");
	pantalla_Clear();
	exit(EXIT_SUCCESS);
}

/**********************************************************/
void 	ADP_CerrarConexion( tSocket *sockIn )
{/*Cerrar el socket correspondiente y tomar la accion correspondiente*/
	conexiones_CerrarSocket(ADP.m_ListaSockets,sockIn,&ADP.m_ultimoSocket);
}

/**********************************************************/
float	ADP_CalcularCargaPromReal()
{
	float fCarga;
	char loadavg[50+1] ;
	FILE *archivo;
	
	
	
	if ((archivo = fopen("/proc/loadavg", "r")) == NULL) 
	{
		Log_printf( log_debug, "Error abriendo archivo del load average" );
		return 0;
	}
	
	fgets(loadavg, 50+2, archivo);
	fclose(archivo) ;
	
	Log_printf(  log_info, "La carga es: %s", loadavg ) ;
	fCarga = atof( strtok( loadavg," ") );
	Log_printf( log_info, "carga: %f\n", fCarga);
	
	
	return fCarga;
} 

/**********************************************************/
int ADP_ForkearPCB( long lpcbid, long *plpid )
{
	char szPCB_ID[10], szIdProceso[10];
	
	sprintf( szPCB_ID, "%ld", lpcbid );
	sprintf( szIdProceso, "%d", _ID_ADP_ );
	
	Log_printf(log_debug,"Voy a hacer el fork del PCB %s", szPCB_ID);
	*plpid = fork();
    
    if( !(*plpid) )
    {
       	Log_printf(log_debug,"Voy a instanciar el PCB");
    	execl( "ppcb", "ppcb",szPCB_ID, szIdProceso, NULL);
        Log_printf(log_debug,"Esto no deberia imprimirse -> FALLA en exec para instanciar PCB");
        Log_printf(log_debug,"Error en exec: %s", strerror(errno));
        
        return ERROR;
  	}
  	else if( (*plpid) ) 
  	{
  		Log_printf(log_debug,"El PID del PCB es: %d ", *plpid );
  		return OK;
    }

}

/**********************************************************/
int	ADP_CrearPCB( long lpcbid, tSocket* pSock, int nMem, long pid )
{/*Si le seteo el socket este no me sirve, porque se cortara la conexion*/
	tunPCB *pcb;
	
	if ( (pcb = pcb_Crear( ADP.m_IP, "", lpcbid, 0, NULL, NULL, ADP.m_Q, nMem, pid   )) )
		return lpcb_AgregarALista( &ADP.m_LPL, pcb );
	
	return ERROR;
}
/**********************************************************/
void ADP_LiberarRecursos(int pid){
	tListaPCB lista=NULL;
	tunPCB *pcb;
	int i;
	
	/*Le informo a todas las de LTP que paren*/
	ADP_InformarSuspencion();
	
	for(i=0; i<3; i++){
		switch (i){
			case 0:{lista=ADP.m_LPB;break;}
			case 1:{lista=ADP.m_LPL;break;}
			case 2:{lista=ADP.m_LPE;break;}
		}
		while(lista){
			pcb = lpcb_Datos( lista );
			if(pcb != NULL){
				if(pcb->id == pid){
					Log_printf(log_info,
							"Se elimina PPCB id:%ld de ADP por logout",
							pcb->id);
								
					/*Elimino el proceso PPCB*/
					kill( pcb->pid, SIGTERM );
					switch (i){
						case 0:{
							lpcb_EliminarDeLista( &ADP.m_LPB, pcb->id );
							/*lista = ADP.m_LPB;*/
							break;}
						case 1:{
							lpcb_EliminarDeLista( &ADP.m_LPL, pcb->id );
							/*lista = ADP.m_LPB;*/
							break;}
						case 2:{
							lpcb_EliminarDeLista( &ADP.m_LPE, pcb->id );
							/*lista = ADP.m_LPB;*/
							break;}
					}
				}
			}
			if(lista!=NULL)lista=lpcb_Siguiente(lista);
		}
		lpcb_LimpiarLista(&lista);
	}
	/*Les digo Start a todas las de LTP*/
	ADP_InformarReanudacion();
}

/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
