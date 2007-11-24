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

/****************************************************/
void ADP_TimeOut( tSocket *sockIn )
/*A ver como anda con esto.....*/
{
	ADP_Dispatcher( SIGALRM );
}


void TestPlanificacion()
{
	return;
/*
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
	int 			pidVector[25];
	char 			p[50];
	int				nCantPCBs;
	char			szPCBStates[PAQ_LEN_MSGCTRL+1];


			
	
	
	tunPCB	*ppcb; int i;
	
	ppcb = pcb_Crear( "127.0.0.1", "", 1, 9700, NULL, NULL, 5, 50, 9000 );
	lpcb_AgregarALista( &ADP.m_LPL, ppcb );
	
	ppcb = pcb_Crear( "127.0.0.1", "", 2, 9700, NULL, NULL, -5, 50, 9000 );
	lpcb_AgregarALista( &ADP.m_LPL, ppcb );

	ppcb = pcb_Crear( "127.0.0.1", "", 3, 9700, NULL, NULL, 5, 50, 9000 );
	lpcb_AgregarALista( &ADP.m_LPB, ppcb );

	ppcb = pcb_Crear( "127.0.0.1", "", 4, 9700, NULL, NULL, -5, 50, 9000 );
	lpcb_AgregarALista( &ADP.m_LPE, ppcb );
	
	
		ReducirIP( ADP.m_IP, szIP );
		
		memset( &(szPCBStates[0]), 0, PAQ_LEN_MSGCTRL+1 );
		
		ADP_CargarPCBsStates( &nCantPCBs, szPCBStates );
		
		paq = paquetes_newPaqInfoPCBsStates( szIP, _ID_ADP_, ADP.m_Port,
											nCantPCBs, szPCBStates );
		
		if (1)
		{
			int i = 0;
			int cant ;
			int size = 0;
			short int id;
			char state;
			
			memcpy( &cant, &(paq->id.UnUsed[0]), sizeof(int) );
			for (i = 0; i < cant; i ++)
			{
				memcpy( &id, &(paq->msg[size]), sizeof(short int) );
				size += sizeof(short int);
				memcpy( &state, &(paq->msg[size]), sizeof(char) );
				size += sizeof(char);

			}
		}

	if ( paq ) 
		paquetes_destruir( paq );

		if( (buffPaq = paquetes_newPaqGetPCBsStatesAsStr( szIP, _ID_ADP_, ADP.m_Port )) )
		{
		}
	
	paquetes_newPaqInfoPCBsStatesAsStr( szIP, _ID_ADP_, ADP.m_Port,
											nCantPCBs, szPCBStates );
*/	
}

void InformarLista( tListaPCB Lista )
{
	tunPCB			*pPCB;
	char			szInfo[255], szAux[20];
	
	memset( szInfo, 0, 255 );
	
	if ( Lista == ADP.m_LPE  && ADP.m_LPE )
		strcpy(szInfo, "LPE: ");
	else if ( Lista == ADP.m_LPL && ADP.m_LPE )
		strcpy(szInfo, "LPL: ");
	else if ( Lista == ADP.m_LPB && ADP.m_LPE )
		strcpy(szInfo, "LPB: ");
	
	while( Lista )
	{
		pPCB = lpcb_Datos( Lista );
		
		Log_printf( log_debug, "PCB %ld , Q %d", pPCB->id, pPCB->Q );
		
		sprintf( szAux, "[%ld],", pPCB->id );
			
		strcat( szInfo, szAux );					
		
		Lista = lpcb_Siguiente( Lista );
	}
	
	ADP_printToWin( ADP.m_pwInfo, szInfo );
}

/*------------------------------------------------------------------*/
void ADP_DesactivarAlarma()
{
	return;
/*	
	int nRestoAlarma;
	Log_printf( log_debug, "Se desactiva alarma" );
	
	g_nAlarmaActiva = 0;
	nRestoAlarma = alarm( 0 );
*/	
}

/*------------------------------------------------------------------*/
void ADP_ActivarAlarma()
{
	return;
/*	
	if ( g_nAlarmaActiva )
		return;
		
	Log_printf( log_debug, "Se Activa alarma" );
	alarm( 1 );
	signal(SIGALRM, &ADP_Dispatcher ); 
	
	g_nAlarmaActiva = 1;
*/	
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
	char	szInfo[255];
	
	memset(szInfo,0,255);
	
	if ( ADP_SeCumplioQ() )
	{
		Log_log( log_debug, "<< Entra al Dispatcher >>" );
		ADP_printToWin( ADP.m_pwLogger, "<< Entra al Dispatcher >>" );
		ventana_Clear( ADP.m_pwInfo );
		
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

		Log_printf( log_debug, "LPB" );
		InformarLista( ADP.m_LPB );
		
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
		ADP_printToWin( ADP.m_pwLogger, "<< Sale del Dispatcher >>" );
		sprintf( szInfo, "Cant PCBs a aceptar %d, Mem Disp %d", ADP.m_nCantDisp, ADP.m_nMemDisp );
		ADP_printToWin( ADP.m_pwInfo, szInfo );
	}
	
	if ( ADP_SeCumplioTimerAverage() )
	{
		/*En este punto leo la config online*/
		ADP_LeerConfigOnline();
		
		if ( ADP_EstoyCargado() )
		{
			ADP_MigrarPCBPesado();/*No es el mas pesado, es el que le resta mas tiempo para terminar*/
		}
		
		g_lTime2 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
	}
	
	/*Si entro aca quiere decir que se cumplio el timer asi que reseteo la senial*/
	ADP_ActivarAlarma();
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
			/*ADP_printToWin( ADP.m_pwLogger, "se eligio un pcb que todavia tiene el sock en null" );*/
									
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
			ADP_printfToWin( ADP.m_pwLogger, "envio Suspension al pcb %ld", pPCB->id  );
	
			if ( conexiones_sendBuff( pPCB->pSocket, (const char*) paquetes_newPaqSuspendPCBAsStr( szIPReducido, _ID_ADP_, ADP.m_Port ), 
					PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
			{
				Log_logLastError("enviando suspend_pcb");
				ADP_CerrarConexion( pPCB->pSocket );
				return;
			}
		}
				
		Lista = lpcb_Siguiente( Lista );
	}
	/*usleep(1000);*/
	  
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
		ADP_printfToWin( ADP.m_pwLogger,"Envio Start al PCB: %ld por fd = %d", 
						pPCB->id, pPCB->pSocket->descriptor );
						
		ReducirIP( ADP.m_IP, szIPReducido );	
		if ( conexiones_sendBuff( pPCB->pSocket, (const char*) paquetes_newPaqExecPCBAsStr( szIPReducido, _ID_ADP_, ADP.m_Port ), 
				PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
		{
			Log_logLastError("enviando exec_pcb");
			/*ADP_printToWin( ADP.m_pwLogger, "Error enviando exec" );*/
			ADP_CerrarConexion( pPCB->pSocket );
			return;
		}
		/*Log_printf( log_debug,"Se envio un EXEC al PCB %d", pPCB->id);*/
		
		Lista = lpcb_Siguiente( Lista );
	}
}



/*************************************************************/
int		ADP_EstoyCargado()
{
	float fCarga = ADP_CalcularCargaPromReal();
	 
	if ( fCarga >= ADP.m_fLimite2 )
	{
		Log_printf( log_info, "Carga prom real = %f, L2 = %f", fCarga, ADP.m_fLimite2  );
		return 1;
	}
	
	return 0;
}


/*************************************************************/
int	ADP_MigrarPCBPesado() 
{/*Asumo que se refiere a cualquier PCB, este en el estado que este*/
	int nMayor = -1000;
	tunPCB		*pcb1 = NULL;
	
	if ( ADP_CantidadPCBs() <= 1 )
	{
		Log_log( log_info, "la cantidad de pcbs es <= 1. Entonces no quito ninguno" );
		/*ADP_printToWin( ADP.m_pwLogger, "cant. de pcbs <= 1. no quito ninguno" );*/
		return OK;
	}
	
	pcb1 = ADP_BuscarPCBMayorTRestante( ADP.m_LPL, &nMayor, pcb1 );
	pcb1 = ADP_BuscarPCBMayorTRestante( ADP.m_LPE, &nMayor, pcb1 );
	pcb1 = ADP_BuscarPCBMayorTRestante( ADP.m_LPB, &nMayor, pcb1 );
	
	if ( pcb1 )
	{
		/*Este es el que tengo que quitar*/
		if ( pcb1->pSocket )
		{
			Log_printf( log_info,"Decido sacar al pcb %ld", pcb1->id );
			ADP_printfToWin( ADP.m_pwLogger,"Decido sacar al pcb %ld", pcb1->id );
			
			conexiones_CerrarSocket(ADP.m_ListaSockets, pcb1->pSocket, &ADP.m_ultimoSocket);
			ADP_EliminarDeLista( pcb1->id );
		}
		else
		{
			/*fuerzo la migracion*/
			kill( pcb1->pid, SIGUSR2 );
			ADP_EliminarDeLista( pcb1->id );
			/*Log_printf( log_info,"Decido sacar al pcb %ld, PERO el socket esta en NULL!!", pcb1->id );*/
			/*ADP_printfToWin( ADP.m_pwLogger,"Decido sacar al pcb %ld, PERO el socket esta en NULL!!", pcb1->id );*/
			return ERROR;
		}
	}
	
	return OK;
}

/**********************************************************/
tunPCB* ADP_BuscarPCBMayorTRestante( tListaPCB Lista, int *nMayor, tunPCB* pcb1 )
{
	int 			len; 
	int 			nCont = 0;
	tunPCB			*pPCB;
	tunPCB			*pPCBMayorTR = pcb1;
	unsigned char	szIPReducido[4];
	int				nTimeRemaining = 1000;
	char			szArchivo[255];
	tConfig 		*cfg;
					
	while( Lista )
	{
		pPCB = lpcb_Datos( Lista );
		
		if ( !pPCB->pSocket )
		{
			Log_printf( log_error, "en el control del L2, salteo un pcb que todavia no se reconecto" );
			Lista = lpcb_Siguiente( Lista );
			continue;/*Esto soluciona lo que me parece que le pasa a Ale*/
		}
		
		/*le mando la senial SIGUSR1 para que escriba el time remaining en el achivo de config.pcb*/
		kill( pPCB->pid, SIGUSR1 );
		/*usleep(1000);*/ /*Por las dudas, le doy un tiempo para que grabe*/
	
		ArmarPathPCBConfig( szArchivo, pPCB->id,sizeof(szArchivo) );
			
		if ( (cfg = config_Crear( szArchivo, _PPCB_ )) )
		{
			nTimeRemaining = config_GetVal_Int(cfg,_PPCB_,"TMP_REST");
			config_Destroy( cfg );
			cfg = NULL;
		}
		else
		{
			Log_printf( log_error, "No se pudo leer el %s!!", szArchivo );
			ADP_printfToWin( ADP.m_pwLogger, "No se pudo leer el %s!!", szArchivo );
			nTimeRemaining = 1000;
		}
		/*Acumular el pcb de mayor tiempo restante y devolverlo como retorno
		 * por default se seta el retorno con el pcb1 que viene como parametro
		 * La comparacion se hace con el param *nMayor*/
		 
		 if ( nTimeRemaining > *nMayor)
		 {
		 	*nMayor = nTimeRemaining;
		 	pPCBMayorTR = pPCB;
		 }
		
		Lista = lpcb_Siguiente( Lista );
	}
	
	return pPCBMayorTR;
}

/**********************************************************/
int ADP_CantidadPCBs()
{
	int nRet = 0;
	
	nRet += lista_contar( &ADP.m_LPL );
	nRet += lista_contar( &ADP.m_LPE );
	nRet += lista_contar( &ADP.m_LPB );
	
	return nRet;
}

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
		Log_log( log_warning, "Recibo senial SIGUSR1");
		ADP_printToWin( ADP.m_pwLogger, "Recibo senial SIGUSR1");
		
		ADP_DesactivarAlarma();
		ADP_ImprimirInfoCtr();
		ADP_ActivarAlarma();
		
		signal(SIGUSR1, &ADP_ProcesarSeniales );
	}
	else if ( senial == SIGUSR2 )
	{
	}
	else if ( senial == SIGHUP )
	{
	}
	else if ( senial == SIGINT || senial == SIGTERM )
	{	
		Log_log( log_warning, "Recibo senial SIGTERM o SIGINT");		
		ADP_Salir();
	}
	else if ( senial == SIGCHLD )
	{	
		Log_log( log_warning, "Recibo senial SIGCHILD");
		ADP_printToWin( ADP.m_pwLogger, "Recibo senial SIGCHILD");
		
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

/**********************************************************/
void ADP_ImprimirInfoCtr()
{
	int i;
	
	Log_log(log_info, "  Se imprime la informacion de control del ADP  ");
	
	InfoCtr_Inicializar("ADP","1");

	ADP_InfoLista( ADP.m_LPL, LISTO );
	ADP_InfoLista( ADP.m_LPE, EJECUTANDO );
	ADP_InfoLista( ADP.m_LPB, BLOQUEADO );

	InfoCtr_CerrarInfo();
}

/**********************************************************/
void ADP_InfoLista( tListaPCB Lista, tState state )
{
	char szArchivo[ 255 ];
	char szCmd[50];
	char szUsr[50];
	char szEstado[50];
	tunPCB	*pcb;
	char 	*tmp;
	tConfig *cfg;
	
	if ( state == LISTO )
	{
		strcpy( szEstado, "Listo" );
		InfoCtr_printf( log_info, "Cola de PCBs Listos (LPL)" );
	}
	else if ( state == EJECUTANDO )
	{
		strcpy( szEstado, "Ejecutando" );
		InfoCtr_printf( log_info, "Cola de PCBs Ejecutando (LPE)" );
	}
	else if ( state == BLOQUEADO )
	{
		strcpy( szEstado, "Bloqueado" );
		InfoCtr_printf( log_info, "Cola de PCBs Bloqueados (LPB)" );
	}
	
	while( Lista )
	{
		pcb = lpcb_Datos( Lista );
		
		ArmarPathPCBConfig( szArchivo, pcb->id,sizeof(szArchivo) );
		
	
		if ( !(cfg = config_Crear( szArchivo, _PPCB_ )) ) 
			continue;
		
		if ( (tmp = config_GetVal( cfg, _PPCB_, "CREATORID" ) ) )
		{
			strcpy( szUsr, tmp );
		}
		if ( (tmp = config_GetVal( cfg, _PPCB_, "COMANDO" ) ) )
		{
			strcpy( szCmd, tmp );
		}
		config_Destroy( cfg );
		
		InfoCtr_printf( log_info, "PCB.id = %ld, Usr = %s, estado = %s,  Cmd = %s", pcb->id, szUsr, szEstado, szCmd );
		
		if ( state == EJECUTANDO )
		{
			InfoCtr_printf( log_info, "Q rest. = %d", pcb->Q );
		}
			
		Lista = lpcb_Siguiente( Lista );
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
		ADP.m_nCantDisp = ADP.m_nCantPCBs;
		
		ADP_CrearGraficos(1);
				
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
			ADP_printToWin( ADP.m_pwLogger, "No se pudo establecer conexion con el ACR!!!" );
			return ERROR;
		}
		
		signal(SIGALRM, ADP_ProcesarSeniales);
		signal(SIGCHLD, ADP_ProcesarSeniales);
		
		ahoraLocal = localtime(&ahoraGlobal);
		signal(SIGTERM, ADP_ProcesarSeniales);
		signal(SIGINT, ADP_ProcesarSeniales);
	
	
		if ( !g_lTime1 )
			g_lTime1 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
		
		if ( !g_lTime2 )
			g_lTime2 = (ahoraLocal->tm_hour *60*60)+ (ahoraLocal->tm_min*60) + ahoraLocal->tm_sec;
		
		ADP_ActivarAlarma();
		signal( SIGALRM, &ADP_Dispatcher );
		signal(SIGUSR1, &ADP_ProcesarSeniales ); 
	
		return OK;
		
	}while(0);
	
	if ( errno )
	{
		Log_log( log_error, conn_getLastError() );
	}
	else
	{
		printf("No se puede crear el ADP: revise el archivo de configuracion\n");
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
		
		if ( (tmp = config_GetVal( cfg, _ADP_, "L1" ) ) )
		{
			ADP.m_fLimite1 = atof( tmp );
		}
		
		if ( (tmp = config_GetVal( cfg, _ADP_, "L2" ) ) )
		{
			ADP.m_fLimite2 = atof( tmp );
		}
		
		ADP.m_nCantPCBs = config_GetVal_Int( cfg, _ADP_, "CantPCBs" );

		config_Destroy(cfg);

		return OK;
		
	}while(0);
	
	return ERROR;
}

/**********************************************************************/
int ADP_LeerConfigOnline()
{
	char 	*tmp;
	tConfig *cfg;
	int		Q;
	
	do
	{		
		if ( !(cfg = config_Crear( "config", _ADP_ )) ) 
			break;
		
		/*Levanto la configuracion*/
		Q = config_GetVal_Int( cfg, _ADP_, "Q" );
		
		if ( ADP.m_Q != Q )
		{
			Log_printf( log_info, "Se cambia el Q de %d a %d", ADP.m_Q, Q );
			ADP_printfToWin( ADP.m_pwLogger, "Se cambia el Q de %d a %d", ADP.m_Q, Q );
			ADP.m_Q = Q;
		}

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
		ADP_printToWin( ADP.m_pwLogger, "Conexion establecida con el ACR!" );
		sockIn->callback = &ADP_AtenderACR;
		
		/*TestPlanificacion();*/
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
		{
			Log_printf( log_debug, "ya tengo el pcb %ld en alguna lista", pcb->id );
			pcb->pSocket = sockIn;
			pcb->Registro[0] = CONECTADO;
		}
		
		Log_log( log_debug, "PCB me manda un ping para establecer conexion" );
		
		sockIn->callback = &ADP_AtenderPCB;
		
		/*Mando el Pong al ADP*/
		paquetes_destruir( paq );
		paq = NULL; /*Se puede hacer una funcion en paquetesGeneral que devuelve un char directamente*/
		
		if ( !(paq  = paquetes_newPaqPong( szIP, _ID_ADP_, conexiones_getPuertoLocalDeSocket(sockIn) )) )
		{
			Log_log( log_error, "Error enviando pong al PPCB" );
			/*ADP_printToWin( ADP.m_pwLogger, "Error enviando pong al PPCB" );*/
		}
			
		Log_log( log_debug, "Le respondo con un pong" );
		nSend = conexiones_sendBuff( sockIn, (const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
		
		
		
		if ( nSend != PAQUETE_MAX_TAM )
		{
			Log_logLastError( "error enviando pong " );
			/*ADP_printToWin( ADP.m_pwLogger, "Error enviando pong al PPCB" );*/
		}	

		/*Espero y le mando un suspend para que se ponga en listo el pcb*/
		if ( pcb && pcb->Registro[1] != BLOQUEADO )
		{
			/*usleep( 1000 );*/			
			if ( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqSuspendPCBAsStr( szIP, _ID_ADP_, ADP.m_Port ), 
					PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
			{
				Log_logLastError("enviando suspend_pcb para que cambie su estado interno");
			}
		}
		/**/
			
		
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
	int 			pidVector[25];
	int 			i;
	char 			p[50];
	int				nCantPCBs;
	char			szPCBStates[PAQ_LEN_MSGCTRL+1];
	tunPCB			*pcb;
	int			pcb_id;
	unsigned char	id_proceso;
	unsigned short 	puerto;


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
		ADP_printToWin( ADP.m_pwLogger, "el ACR me manda un GetPerformance" );

		fCargaProm = ADP_CalcularCargaPromReal();
		
		/*CantPCB = (float)ADP.m_nMemMax / fCargaProm; Ubiese estado bueno haberlo pensado como funcion, pero el cambio es facil igual*/
		if( fCargaProm > ADP.m_fLimite1 ){
			CantPCB = 0.0;		/*lo dice el enunciado Leonardoooo!! lo digo con onda, pero cuando hiciste el arreglo lo volviste a leer?*/
		}else{
			CantPCB = ADP.m_nCantDisp;
		} 
		Log_printf(log_debug,"CantPCB(%d), ADP.m_nMemMax(%.5f), fCargaProm(%.5f)",
					CantPCB,(float)ADP.m_nMemMax, fCargaProm);
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
		Log_log( log_debug, "el ACR me manda un kill por end_sesion" );
		/*ADP_printToWin( ADP.m_pwLogger, "el ACR me manda un kill por end_sesion" );*/
		
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
	/* el acr me pide los estados de los pcbs */
	else if( IS_PAQ_GET_PCBS_STATES( paq ) )
	{
		Log_log( log_debug, "el ACR me manda un GetPCBsStates" );
		ADP_printToWin( ADP.m_pwLogger, "el ACR me manda un GetPCBsStates" );

		ReducirIP( ADP.m_IP, szIP );
		
		memset( &(szPCBStates[0]), 0, PAQ_LEN_MSGCTRL+1 );
		
		ADP_CargarPCBsStates( &nCantPCBs, szPCBStates );
		
		if( (buffPaq = paquetes_newPaqInfoPCBsStatesAsStr( szIP, _ID_ADP_, ADP.m_Port,
											nCantPCBs, szPCBStates )) )
		{
			Log_log( log_debug, "Envio un InfoPCBsStates" );
			nSend = conexiones_sendBuff( sockIn, buffPaq, PAQUETE_MAX_TAM );
		}
	}
	else if( IS_PAQ_SOL_CONCEDIDO(paq) )
	{
		Log_log(log_debug, "recibo PAQ_SOL_CONCEDIDO");
		ADP_printToWin( ADP.m_pwLogger, "recibo PAQ_SOL_CONCEDIDO");
		
		paquetes_ParsearSolConcedido(buffer,szIP,&id_proceso,&puerto,&pcb_id);
		
		if( pcb = lpcb_BuscarPCBxid( &ADP.m_LPB, (long)pcb_id ) ) /*Busco el pcb y actualizo el socket*/
		{
			nSend = conexiones_sendBuff( pcb->pSocket, (const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
		
			if(nSend != PAQUETE_MAX_TAM)
			{
				Log_logLastError( "error al enviar PAq_SOL_CONCEDIDO al ACR" );
				/*ADP_printToWin( ADP.m_pwLogger, "error al enviar Paq_sol_concedido al ACR");*/
			}

			lpcb_PasarDeLista( &ADP.m_LPB, &ADP.m_LPL, pcb->id );
			Log_printf(log_info,"se paso de bloqueado a listos el ppcb id %ld",pcb->id);
		}
		else
		{
			Log_printf(log_error,"no se encontro el ppcb en la lista de bloqueados");
		}
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
	
}

/**********************************************************/
void ADP_CargarPCBsStates( int *pnCantPCBs, char szPCBStates[PAQ_LEN_MSGCTRL+1] )
{
/*	
	BLOQUEADO = 1,
	LISTO = 2,
	EJECUTANDO = 3,
*/
	int nSize = 0;
	(*pnCantPCBs) += ADP_LlenarBuffPCBsStates( ADP.m_LPL, &nSize, szPCBStates, LISTO ); 
	(*pnCantPCBs) += ADP_LlenarBuffPCBsStates( ADP.m_LPE, &nSize, szPCBStates, EJECUTANDO );
	(*pnCantPCBs) += ADP_LlenarBuffPCBsStates( ADP.m_LPB, &nSize, szPCBStates, BLOQUEADO );
}
/**********************************************************/
int ADP_LlenarBuffPCBsStates( tListaPCB Lista, int *pnSize, char szPCBStates[PAQ_LEN_MSGCTRL+1], tState state )
{
	int 			nCont = 0;
	tunPCB			*pPCB;
	short int		nid;/*con int el size me daba 4, state tb, asi que tenia que bajar bytes!*/
	char			c = (char) state;
					
	while( Lista )
	{
		nCont++;
		
		pPCB = lpcb_Datos( Lista );
		nid = (short int) (pPCB->id);
		
		memcpy( &(szPCBStates[ *pnSize ]), &nid, sizeof(short int) );
		(*pnSize) += sizeof(short int);

		memcpy( &(szPCBStates[ *pnSize ]), &c, sizeof(char) );
		(*pnSize) += sizeof(char);
		
		if ( *pnSize >= PAQ_LEN_MSGCTRL )
		{
			Log_printf( log_warning, "son muchos pcbs!" );
			break;/*no quiero segFault!!*/
		}
		
		Lista = lpcb_Siguiente( Lista );
	}
	
	return nCont;
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
	char			szPathArch[255]; 
	long			lpcb_id;
	tunPCB			*pPCB;
	int 			nSend;
	unsigned char	szIP[4];

	memset(szPathArch,0,255);
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
		/*ADP_printToWin( ADP.m_pwLogger, "Llega un paq Migrar..." );*/
		
		ADP_DesactivarAlarma();
		
		memcpy( &lpcb_id, paq->msg, sizeof(long) );
		bzero(szPathArch,sizeof(szPathArch));
		
		Log_printf( log_debug, "...del pcb %ld", lpcb_id );
		ADP_printfToWin( ADP.m_pwLogger, "Recibiendo al pcb %ld por migracion", lpcb_id );
		
		ArmarPathPCBConfig( szPathArch, lpcb_id,sizeof(szPathArch) );
		
		
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
			/*ADP_printToWin( ADP.m_pwLogger,"No pude abrir el archivo de la migracion" );*/
			
			/*Envio el msj de migracion Fault*/
			Log_log( log_debug, "envio migrar Fault" );
			ReducirIP( ADP.m_IP, szIP);
			if ( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqMigrarFault( szIP, _ID_ADP_, ADP.m_Port ), 
					PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
			{
				Log_logLastError("enviando migrar_Fault");
				/*ADP_printToWin( ADP.m_pwLogger, "enviando migrar_Fault" );*/
			}
			
		}
	}
	else if(IS_PAQ_PRINT(paq))
	{/*me llega el print, se lo reenvio al ACR*/
		ADP_DesactivarAlarma();/*no quiero tener lios al manipular las colas*/
		
		Log_log( log_debug, "Mando PRINT al ACR" );
		ADP_printToWin( ADP.m_pwLogger, "Mando PRINT al ACR" );
		
		nSend = conexiones_sendBuff( ADP.m_ListaSockets[SOCK_ACR], (const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
					
		if(nSend != PAQUETE_MAX_TAM)
		{
			Log_logLastError( "error al enviar PRINT al ACR" );
			/*ADP_printToWin( ADP.m_pwLogger, "error al enviar PRINT al ACR" );*/
		}
		
		memcpy( &lpcb_id, paq->id.UnUsed, sizeof(long) );
		
		if( lpcb_id  != 0 )
		{/*Entonces es el print final*/
			Log_printf(log_debug,"Finalizo el PCB %ld", lpcb_id );
			ADP_printfToWin( ADP.m_pwLogger,"Finalizo el PCB %ld", lpcb_id );
			
			if ( (pPCB = ADP_BuscarPCB( lpcb_id ) ) )
			{
				Log_printf(log_debug,"Mando el kill al PCB %ld", lpcb_id );
				kill( pPCB->pid, SIGTERM );
				
				ADP_EliminarDeLista( pPCB->id );
			}	
		}
		
		ADP_ActivarAlarma();
	}
	
	else if(IS_PAQ_DEV(paq))
	{/*me llega el dev, se lo reenvio al ACR*/
		int nSend;
		
		int id;
		memcpy( &id,&(paq->msg[SOLDEV_POS_PPCBID]), sizeof( int ) );
		/*Log_printf(log_error,"es una prueba %i%i%i",id,id,id);*/
		
		Log_log( log_info, "Mando DEV al ACR" );
		ADP_printToWin( ADP.m_pwLogger, "Mando DEV al ACR" );
		
		nSend = conexiones_sendBuff( ADP.m_ListaSockets[SOCK_ACR], (const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
					
		if(nSend != PAQUETE_MAX_TAM)
		{
			Log_logLastError( "error al enviar DEV al ACR" );
			/*ADP_printToWin( ADP.m_pwLogger, "error al enviar DEV al ACR" );*/
		}
	}
	
	/*Manejo de recursos - Buscar el pcb y pasarlo a la LPB, ver el tad DatosPCB funcion que pasa de listas- 
	 * Hay dos funciones en esta lib que busca el pcb en todas las listas por id o por socket
	 * Si entendi, esto es tuyo miguel*/
	else if( IS_PAQ_SOL( paq ) )
	{
		unsigned char IP[4];
		unsigned char id_Proceso;
		unsigned short int puerto;
		int PPCB_id;
		tRecurso recursoSolicitado;
		unsigned char szIP[4];
		memset( szIP, 0, 4 );
		ReducirIP(ADP.m_IP,szIP);
		
		Log_log( log_info, "Recibo un PAQ_SOL del PCB" );
		ADP_printToWin( ADP.m_pwLogger, "Se recibe un paq Sol" );
	
		paquetes_ParsearSol(buffer, (unsigned char*)IP,&id_Proceso, &puerto, &PPCB_id, &recursoSolicitado);
		lpcb_PasarDeLista(&ADP.m_LPE, &ADP.m_LPB, PPCB_id);
		
		if( pPCB = lpcb_BuscarPCBxid( &ADP.m_LPB, PPCB_id ) ) {
			ADP.m_nMemDisp += pPCB->MemoriaRequerida;
			/*se devuelve la memoria*/
			Log_log(log_info,"se devuelve la memoria al pasar a bloqueados");
		} else {
			Log_printf(log_error,"no se encontro el ppcb en la lista de bloqueados");
		}		
		
		Log_printf(log_info, "se movio el PPCB id = %d de la LPE a la LPB", PPCB_id);
		if ( conexiones_sendBuff( ADP.m_ListaSockets[SOCK_ACR], (const char*) paquetes_newPaqSolAsStr(szIP, (unsigned char)_ADP_, ADP.m_Port, PPCB_id, recursoSolicitado),PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
		{
			Log_logLastError("error enviando recurso solicitado al ACR");
			/*ADP_printToWin( ADP.m_pwLogger, "error enviando recurso solicitado al ACR");*/
		}
	}

	
	if ( paq ) 
		paquetes_destruir( paq );
	
}

/***********************************************************/
tunPCB* ADP_BuscarPCB( long id )
{
	tunPCB *pPCB;
	
	if ( (pPCB = lpcb_BuscarPCBxid( &ADP.m_LPL, id ) ) ||
		(pPCB = lpcb_BuscarPCBxid( &ADP.m_LPE, id ) ) ||
		(pPCB = lpcb_BuscarPCBxid( &ADP.m_LPB, id ) ) )
		return pPCB;
	return NULL;
}

/***********************************************************/
tunPCB* ADP_BuscarPCBxSocket( tSocket* pSocket )
{
	tunPCB *pPCB;
	
	if ( (pPCB = lpcb_ObtenerPCBXSock( &ADP.m_LPL, pSocket ) ) ||
		(pPCB = lpcb_ObtenerPCBXSock( &ADP.m_LPE, pSocket ) ) ||
		(pPCB = lpcb_ObtenerPCBXSock( &ADP.m_LPB, pSocket ) ) )
		return pPCB;
	return NULL;
}

/***********************************************************/
void ADP_EliminarDeLista( long id )
{
	tunPCB *pcb;
	
	if ( lpcb_BuscarPCBxid( &ADP.m_LPL, id )  )
	{
		lpcb_EliminarDeLista( &ADP.m_LPL, id );
		Log_printf(log_debug,"Elimino el PCB de la lista LPL");
		ADP.m_nCantDisp++;
		return;
	}
	if ( (pcb = lpcb_BuscarPCBxid( &ADP.m_LPE, id )) )
	{
		/*Tengo que devolver la memoria que estaba usando!!!*/
		ADP.m_nMemDisp += pcb->MemoriaRequerida;
				
		lpcb_EliminarDeLista( &ADP.m_LPE, id );
		Log_printf(log_debug,"Elimino el PCB de la lista LPE");
		ADP.m_nCantDisp++;
		return;
	}
	if ( lpcb_BuscarPCBxid( &ADP.m_LPB, id )  )
	{
		lpcb_EliminarDeLista( &ADP.m_LPB, id );
		Log_printf(log_debug,"Elimino el PCB de la lista LPB");
		ADP.m_nCantDisp++;
		return;
	}
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
	char			szPathArch[255];	 
	unsigned char	szIP[4];
	long			lpcb_id;
	int				nMemoria;
	long			lpid;
	tState			pcb_state;

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
		/*ADP_printToWin( ADP.m_pwLogger, "Error al crear el paquete en ADP_RecibirArchivo" );*/
	}

	if ( IS_PAQ_ARCHIVO( paq ) )
	{/*Llega el paq archivo -> persisto el contenido*/
		Log_log( log_debug, "Llega un PAQ_ARCHIVO" );
		ADP_printToWin( ADP.m_pwLogger, "Llega un PAQ_ARCHIVO" );
		
		bzero(szPathArch,sizeof(szPathArch));
		lpcb_id = (long)sockIn->extra;
		ArmarPathPCBConfig( szPathArch, lpcb_id,sizeof(szPathArch) );
		
		if ( (arch = fopen( szPathArch, "ab+" )) ){
			fprintf( arch, "%s", paq->msg );
			fclose(arch);
		}else{
			Log_logLastError( "No pude abrir el archivo de la migracion" );
			/*ADP_printToWin( ADP.m_pwLogger, "Error No pude abrir el archivo de la migracion" );*/

			/*Envio el msj de migracion Fault*/
			Log_log( log_debug, "envio migrar Fault" );
			ReducirIP( ADP.m_IP, szIP);
			if ( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqMigrarFault( szIP, _ID_ADP_, ADP.m_Port ), 
					PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
			{
				Log_logLastError("enviando migrar_Fault");
				/*ADP_printToWin( ADP.m_pwLogger, "enviando migrar_Fault" );*/
			}
			sockIn->callback = 	&ADP_AtenderPCB;
		}
	}
	else if ( IS_PAQ_FIN_MIGRAR( paq ) )
	{
		Log_log( log_debug, "llega un PAQ_FIN_MIGRAR" );
		ADP_printToWin( ADP.m_pwLogger, "llega un PAQ_FIN_MIGRAR" );
		
		lpcb_id = (long)sockIn->extra;
		sockIn->callback = 	&ADP_AtenderPCB;
		
		/*Envio el msj de migracion OK*/
		Log_log( log_debug, "envio migrar ok" );
		ReducirIP( ADP.m_IP, szIP);
		if ( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqMigrarOKAsStr( szIP, _ID_ADP_, ADP.m_Port ), 
				PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
		{
			Log_logLastError("enviando migrar_ok");
			/*ADP_printToWin( ADP.m_pwLogger, "error enviando migrar_ok");*/
		}
		else
		{
			if ( ADP_ForkearPCB( lpcb_id, &lpid ) == OK )
			{
				memcpy( &nMemoria, &(paq->id.UnUsed[0]), sizeof(int) );
				memcpy( &pcb_state, &(paq->msg[0]), sizeof(tState) );
				
				ADP_CrearPCB( lpcb_id, sockIn, nMemoria, lpid, pcb_state );/*Lo crea y lo agrega a la lista de listos*/
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
					/*ADP_printToWin( ADP.m_pwLogger, "error enviando migrar_Fault");*/
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
	/* Logear salida, hacer un clean up? 
	ADP_MigrarLosPCBs( ADP.m_LPL );
	ADP_MigrarLosPCBs( ADP.m_LPE );
	ADP_MigrarLosPCBs( ADP.m_LPB );
	*/
	conexiones_CerrarLista( 0, &ADP.m_ultimoSocket, ADP.m_ListaSockets );

	lpcb_LimpiarLista( &ADP.m_LPL );
	lpcb_LimpiarLista( &ADP.m_LPE );
	lpcb_LimpiarLista( &ADP.m_LPB );
/*
	Log_log(log_info,"matando los pcbs de LPL");
	lpcb_MatarPCBs( &ADP.m_LPL );
	Log_log(log_info,"matando los pcbs de LPE");
	lpcb_MatarPCBs( &ADP.m_LPE );
	Log_log(log_info,"matando los pcbs de LPB");
	lpcb_MatarPCBs( &ADP.m_LPB );
*/
	Log_log(log_info,"Fin de la ejecucion");
	ventana_Clear( ADP.m_pwMain );
	ventana_Clear( ADP.m_pwInfo );
	ventana_Clear( ADP.m_pwLogger );
	pantalla_Clear();
	exit(EXIT_SUCCESS);
}

/**********************************************************/
void 	ADP_CerrarConexion( tSocket *sockIn )
{/*Cerrar el socket correspondiente y tomar la accion correspondiente*/
	tunPCB	*pPCB;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	if ( (pPCB = ADP_BuscarPCBxSocket( sockIn )) && ADP.m_ListaSockets[SOCK_ACR] && pPCB->Registro[0] == CONECTADO )
	{/*Muerte inesperada del PCB, le mando un print al ACR con el id del PCB*/
		memset( szIP, 0, 4 );
		
		ReducirIP( ADP.m_IP, szIP );
		
		Log_printf( log_info, "mando un print al ACR por muerte inesperada del PCB: %ld", pPCB->id );
		
		pPaq = paquetes_newPaqPrint( szIP, (unsigned char)_ADP_, ADP.m_Port, -1, "", "Muerte inesperada" );/*Si esto provoca error, analizar "" que se pasan*/
		memcpy( &(pPaq->id.UnUsed), &(pPCB->id), sizeof(long) );
		
		Log_log( log_debug, "Mando PRINT al ACR" );
		nSend = conexiones_sendBuff( ADP.m_ListaSockets[SOCK_ACR], (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
		if ( nSend != PAQUETE_MAX_TAM )
		{
			Log_logLastError( "error enviando PRINT al ACR" );
		}	
		paquetes_destruir( pPaq );
		
		ADP_EliminarDeLista( pPCB->id );
	}
	Log_log(log_warning,"se cierra socket PPCB");
	conexiones_CerrarSocket(ADP.m_ListaSockets,sockIn,&ADP.m_ultimoSocket);
}

/**********************************************************/
float	ADP_CalcularCargaPromReal()
{
	float fCarga;
	/*char loadavg[50+1] ;*/
	char loadavg[255] ;
	FILE *archivo;
	
	memset( loadavg, 0, 255 );
	
	
	if ((archivo = fopen("/proc/loadavg", "r")) == NULL) 
	{
		Log_printf( log_debug, "Error abriendo archivo del load average" );
		ADP_printfToWin( ADP.m_pwLogger, "Error abriendo archivo del load average" );
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
int	ADP_CrearPCB( long lpcbid, tSocket* pSock, int nMem, long pid, tState pcb_state )
{/*Si le seteo el socket este no me sirve, porque se cortara la conexion*/
	tunPCB *pcb;

	if ( (pcb = pcb_Crear( ADP.m_IP, "", lpcbid, 0, NULL, NULL, ADP.m_Q, nMem, pid   )) )
	{
		pcb->Registro[1] = pcb_state;
		
		if ( pcb_state == BLOQUEADO )
		{
			ADP.m_nCantDisp--;
			Log_printf( log_debug, "Estado del pcb %ld = %d, Bloqueado", pcb->id,  (int) pcb_state );
			ADP_printfToWin( ADP.m_pwLogger, "Estado del pcb %ld = %d, Bloqueado", pcb->id,  (int) pcb_state );
			return lpcb_AgregarALista( &ADP.m_LPB, pcb );
		}
		else
		{
			ADP.m_nCantDisp--;
			Log_printf( log_debug, "Estado del pcb %ld = %d, lo agrego en Listos", pcb->id,  (int) pcb_state );
			ADP_printfToWin( ADP.m_pwLogger, "Estado del pcb %ld = %d, lo agrego en Listos", pcb->id,  (int) pcb_state );
			return lpcb_AgregarALista( &ADP.m_LPL, pcb );
		}
	}
	
	return ERROR;
}
/**********************************************************/
void ADP_LiberarRecursos(int pid){
	tListaPCB lista=NULL;
	tunPCB *pcb;
	int i;
	char SeElimino = 0;/*Si se elimina rompo los whiles*/
	
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
							SeElimino = 1;
							break;}
						case 1:{
							lpcb_EliminarDeLista( &ADP.m_LPL, pcb->id );
							SeElimino = 1;
							/*lista = ADP.m_LPB;*/
							break;}
						case 2:{
							lpcb_EliminarDeLista( &ADP.m_LPE, pcb->id );
							SeElimino = 1;
							/*lista = ADP.m_LPB;*/
							break;}
					}
				}
			}
			
			if ( SeElimino )
				break;
			
			if(lista!=NULL)lista=lpcb_Siguiente(lista);
		}
		
		/*lpcb_LimpiarLista(&lista);*/
		if ( SeElimino )
			break;
	}
	/*Les digo Start a todas las de LTP*/
	ADP_InformarReanudacion();
}

/************************************************************************/
void ADP_CrearGraficos( int activarGraficos )
{
	#define X_MAIN		1
	#define Y_MAIN		1
	#define ANCHO_MAIN	PANTALLA_COLS
	#define ALTO_MAIN	PANTALLA_ROWS

	#define X_INFO		1
	#define Y_INFO		4
	#define ANCHO_INFO	ANCHO_MAIN
	#define ALTO_INFO	8

	#define X_LOGGER		1
	#define Y_LOGGER		12/*PANTALLA_ROWS - (ALTO_INFO + 6)*/
	#define ANCHO_LOGGER	PANTALLA_COLS
	#define ALTO_LOGGER		12/*PANTALLA_ROWS - (Y_LOGGER + 3)*/
	
	char	szTitle[PANTALLA_COLS+1];
	
	sprintf( szTitle, "[B&RR - ADP] %s: %d", ADP.m_IP, ADP.m_Port );

	
	if ( 1 ) /*por si lo quiero desactivar en el futuro*/
	{
		pantalla_Clear();
		
		ADP.m_pwMain  	= ventana_Crear(X_MAIN, 	Y_MAIN, 	ANCHO_MAIN, 	ALTO_MAIN, 	1, szTitle );
		ADP.m_pwInfo 	= ventana_Crear(X_INFO, 	Y_INFO, 	ANCHO_INFO, 	ALTO_INFO, 	1, "Info");
		ADP.m_pwLogger 	= ventana_Crear(X_LOGGER, 	Y_LOGGER, 	ANCHO_LOGGER, 	ALTO_LOGGER, 1, "Logger");
	}
	else
	{
		ADP.m_pwMain = ADP.m_pwInfo = ADP.m_pwLogger = NULL;
	}
}

/***********************************************************************/
void ADP_printToWin( tVentana *win, char* msg )
{
	time_t		stTime = time( NULL );
	struct tm*	stHoy = localtime( &stTime );
	char szLog[255];
	
	if (win == NULL)
		return;
		
	if ( win == ADP.m_pwLogger )
	{
	  	sprintf( szLog, "%02d:%02d:%02d: %s", 
			stHoy->tm_hour, stHoy->tm_min , stHoy->tm_sec, msg );
		
		ventana_Print( win, szLog );
		fflush(stdout);
	}
	else
	{
		ventana_Print( win, msg );
		fflush(stdout);
	}	
}

/***********************************************************************/
void ADP_printfToWin( tVentana* win, const char* fmt, ... )
{
	va_list args;	
	char buffer[500];
	
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);
	
	ADP_printToWin( win, buffer);
}

/************************************************************8*/
void ADP_MigrarLosPCBs( tListaPCB Lista )
{
	tunPCB			*pPCB;
	
	while( Lista )
	{
		pPCB = lpcb_Datos( Lista );
		
		/*kill( pPCB->pid, SIGUSR2 );*/
		if ( pPCB->pSocket )
		{
			pPCB->pSocket->callback = NULL;
			conexiones_CerrarSocket( ADP.m_ListaSockets, pPCB->pSocket, &ADP.m_ultimoSocket);
		}
				
		Lista = lpcb_Siguiente( Lista );
	}
}

/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
