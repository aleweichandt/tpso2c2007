/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: ACRLib.c
 * 
 * Historia
 * 23/09/2007	GT	Creacion
 *
 ------------------------------------------------------------------ 
 */

#include "ACRLib.h"

/**********************************************************************/
int ACR_Init()
{
	do
	{
		Log_Inicializar( _ACR_ , "1" );
		
		/*inicializa contadores*/
		lContProcesos = 0;
		
		if ( ACR_LeerConfig() == ERROR )
		{
			Log_log( log_error, "Error leyendo la configuracion" ); 
			break;
		}
				
		/* inicializacion de la lista de sockets */
		if ( !(ACR.t_ListaSockets = malloc( MALLOC_SOCKS_INI *sizeof(tSocket*) ) ) ) 
			return ERROR;
			
		memset( ACR.t_ListaSockets, 0, MALLOC_SOCKS_INI * sizeof(tSocket*) );
		
		/*Creo el socket escucha*/
		if ( ! (ACR.t_ListaSockets[ SOCK_ESCUCHA ] = conexiones_crearSockEscucha( 
									&ACR.usi_ACR_Port, 10,	&ACR_AceptarConexion )) )
			break;
			
		ACR.ui_ultimoSocket = SOCK_ESCUCHA;
		
		/* inicializacion de la lista de sockets a adps*/
		lista_inic( &ACR.t_ListaSocketAdp );
		
		/*signals*/
		signal(SIGALRM, ACR_SenialTimer);
		signal(SIGTERM, ACR_ProcesarSeniales);
		signal(SIGUSR1, ACR_ProcesarSeniales);
		signal(SIGINT , ACR_ProcesarSeniales);
		signal(SIGCHLD, ACR_ProcesarSeniales);
	
		return OK;
		
	}while(0);
	
	if ( errno )
	{
		Log_log( log_error, conn_getLastError() );
	}
	else
	{
		printf("No se puede crear el ACR: revise el archivo de configuracion\n");
	}
		
	exit(EXIT_FAILURE);
	
	return ERROR;
}

/**********************************************************************/
void ACR_Salir()
{
	/* Logear salida, hacer un clean up? */
	
	lista_destruir( &ACR.t_ListaSocketAdp );
	
	conexiones_CerrarLista( 0, &ACR.ui_ultimoSocket, ACR.t_ListaSockets );
	free(ACR.t_ListaSockets);
	
	Log_log(log_info,"Fin de la ejecucion");
	Log_Cerrar();
	/*pantalla_Clear();*/
	exit(EXIT_SUCCESS);
}

/**********************************************************************/
int ACR_LeerConfig()
{
	char 	*tmp;
	tConfig *cfg;
	
	do
	{		
		if ( !(cfg = config_Crear( "config", _ACR_ )) ) 
			break;
		
		/*Levanto la configuracion*/
		if ( (tmp = config_GetVal( cfg, _ACR_, "ACR_IP" ) ) )
		{
			strncpy( ACR.sz_ACR_IP, tmp, LEN_IP );
		}
		
		ACR.usi_ACR_Port = config_GetVal_Int( cfg, _ACR_, "ACR_PORT" );
		
		ACR.i_maxLifeTimePPCB = config_GetVal_Int(cfg, _ACR_, "LIFE_TIME_PPCB");
		
		if ( (tmp = config_GetVal( cfg, _ACR_, "PATH_PROG" ) ) )
		{
			if( ExistPath(tmp) != ERROR ){
				strncpy( ACR.sz_programPath, tmp, LEN_DIR );
			}else{
				/* No existe el directorio de Programas */
				Log_logLastError("NO EXISTE el directorio de Programas (!!!)");
				break;
			}
		}
				
		if ( (tmp = config_GetVal( cfg, _ACR_, "PATH_USU" ) ) )
		{
			if( ExistPath(tmp) != ERROR ){
				strncpy( ACR.sz_userPath, tmp, LEN_DIR );
			}else{
				/* No existe el directorio de Usuarios */
				Log_logLastError("NO EXISTE el directorio de Usuarios (!!!)");
				break;
			}				
		}
		
		config_Destroy(cfg);

		return OK;
		
	}while(0);
	
	return ERROR;
}

/**********************************************************************/
int ACR_LeerConfigRuntime()
{
	char 	*tmp;
	tConfig *cfg;
	
	do
	{		
		if ( !(cfg = config_Crear( "config", _ACR_ )) ) 
			break;
		
			
		ACR.i_maxLifeTimePPCB = config_GetVal_Int( cfg, _ACR_, "LIFE_TIME_PPCB" );
		
		config_Destroy(cfg);
		
		return OK;
	}while(0);
	
	return ERROR;
}

/**********************************************************************/
void ACR_ProcesarSeniales( int senial )
/*23/09/2007	GT	La atencion del timer esta en ACR_SenialTimer */
{
	int nstateChld;
 	
 	if ( senial == SIGUSR1 )
	{	/* TODO: Escribir info de estado del sistema */
		Log_log( log_info, "Recibo senial SIGUSR1");		

	}
	else if ( senial == SIGINT )
	{
		Log_log( log_warning, "Recibo senial SIGINT");
		ACR_Salir();
	}
	else if ( senial == SIGTERM )
	{
		Log_log( log_warning, "Recibo senial SIGTRM");
		ACR_Salir();
	}
	else if ( senial == SIGCHLD )
	{	
		Log_log( log_warning, "Recibo senial SIGCHILD");
		wait( &nstateChld );
		if(  WIFEXITED(nstateChld) ){
			Log_printf( log_warning, "Murio un proc hijo en forma normal. ExitStatus: %d",
						WEXITSTATUS(nstateChld));
		}else{
			Log_log( log_warning, "Murio un proc hijo en forma Anormal");
		}
		signal( SIGCHLD, ACR_ProcesarSeniales );
	}
}

/**********************************************************************/
void ACR_SenialTimer( int senial )
{
	if ( senial == SIGALRM ) /*Timer*/
	{
		/*Log_log( log_info, "Recibo senial SIGALRM");*/
		
		ACR_ControlarPendientes();
		
		ACR_PonerTimer();
	}else{
		Log_log(log_warning, "Recibi otra senial en el handler del ALRM");
	}
}

void ACR_SacarTimer(void){
	signal(SIGALRM, ACR_SenialTimer);
	alarm(0);	
}

void ACR_PonerTimer(void){
	signal(SIGALRM, ACR_SenialTimer);
	alarm(ALRM_T);
}

/**********************************************************************/
void ACR_AceptarConexion( tSocket* sockIn )
{
	char szLog[50];
	
	ACR.t_ListaSockets = realloc( ACR.t_ListaSockets, 
								( ++ACR.ui_ultimoSocket + MALLOC_SOCKS_INI ) * sizeof( tSocket* ) );
							
	ACR.t_ListaSockets[ ACR.ui_ultimoSocket ] = conexiones_crearSockAceptado( sockIn, 
																&ACR_HandShake );
	
	if ( !ACR.t_ListaSockets[ ACR.ui_ultimoSocket ] )
	{
		sprintf( szLog, "Rechazada conexion de %s:%d", conexiones_getIpRemotaDeSocket( sockIn ), 
								(int) conexiones_getPuertoRemotoDeSocket( sockIn ) );
		Log_log(log_debug, szLog  );
		
		--( ACR.ui_ultimoSocket );
	}
	else
	{
		sockIn->estado = estadoConn_escuchando;
		
		sprintf( szLog , "Aceptada conexion de %s:%d", 
			conexiones_getIpRemotaDeSocket( ACR.t_ListaSockets[ ACR.ui_ultimoSocket ] ), 
			(int)conexiones_getPuertoRemotoDeSocket( ACR.t_ListaSockets[ ACR.ui_ultimoSocket ] ) );
		
		Log_log( log_debug, szLog );
	}
}

/**********************************************************************/
void ACR_HandShake( tSocket* sockIn )
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ]; 
	unsigned char szIP[4];
	int nSend, bSendPong = FALSE;
	
	
	memset( szIP, 0, 4 );

	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		ACR_CerrarConexion( sockIn );		
	
		return;
	}
	
	paq = paquetes_CharToPaq( buffer );

	if ( IS_ADS_PAQ( paq ) && IS_PAQ_PING ( paq ) )
	{/*Bienvenido ADS*/
		
		Log_log( log_debug, "ADS me manda un ping para establecer conexion" );
		
		ACR.psocketADS = sockIn;
		sockIn->callback = &ACR_AtenderADS;
		sockIn->onClose = &ACR_DesconectarADS;
		sockIn->estado = estadoConn_escuchando;
		
		bSendPong = TRUE;
	} 
	else if( IS_ADP_PAQ(paq) && IS_PAQ_PING( paq ) )
	{/*Bienvenido ADP*/
		
		Log_log( log_debug, "ADP me manda un ping para establecer conexion" );
		
		sockIn->callback = &ACR_AtenderADP;
		sockIn->onClose = &ACR_DesconectarADP;
		sockIn->estado = estadoConn_escuchando;
		
		lista_insertar( &ACR.t_ListaSocketAdp, sockIn, sizeof(tSocket), compararSocket, _SIN_REPET_);
		
		bSendPong = TRUE;
	}
	else if(IS_PPCB_PAQ(paq) && IS_PAQ_PING( paq ) )
	{/*Bienvenido PPCB*/
		
		Log_log( log_debug, "PPCB me manda un ping para establecer conexion" );
		
		sockIn->callback = &ACR_AtenderPPCB;
		sockIn->onClose = &ACR_DesconectarPPCB;
		sockIn->estado = estadoConn_escuchando;
		
		/*TODO: Determinar a que proceso pertenece para asociar el socket*/
		
		bSendPong = TRUE;
	}
	
	
	if( bSendPong )
	{	/*Mando el Pong*/
		paquetes_destruir( paq );
		paq = NULL;
		
		if ( !(paq  = paquetes_newPaqPong( szIP, _ID_ACR_, conexiones_getPuertoLocalDeSocket(sockIn) )) )
		{
			Log_log( log_error, "Error creando pong " );
		}
			
		Log_log( log_debug, "Le respondo con un pong" );
		nSend = conexiones_sendBuff( sockIn, (const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
		
		if ( nSend != PAQUETE_MAX_TAM )
			Log_logLastError( "error enviando pong " );
		
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
}

/**********************************************************************/
void ACR_ControlarPendientes(void)
{
	tListaPpcbAcr 	Lista = ACR.t_ListaPpcbPend;
	tPpcbAcr		*ppcb = NULL;
	time_t			now = time(NULL);
	
	while( Lista )
	{
		ppcb = PpcbAcr_Datos( Lista );
		
		if ( ppcb->sActividad == Estado_Inactivo &&
				difftime( now, ppcb->sFechaInactvdad ) > (double)ACR.i_maxLifeTimePPCB )
		{
			Log_printf(log_info,
				"Se elimina PPCB id:%ld %s de %s, de LPendientes por timeout",
				ppcb->pid, ppcb->szComando, ppcb->szUsuario);
			
			/*Elimino el proceso PPCB*/
			PpcbAcr_EliminarPpcb( &ACR.t_ListaPpcbPend, ppcb->pid );
			kill( ppcb->pidChild, SIGTERM );
			Lista = ACR.t_ListaPpcbPend;
			continue;
			
		}else if ( ppcb->sActividad == Estado_Inactivo )
		{
			Log_printf(log_info,
				"Intento reubicar PPCB id:%ld %s de %s, en nodo de ejecucion",
				ppcb->pid, ppcb->szComando, ppcb->szUsuario);
			
			ACR_DeterminarNodo(ppcb);
			break;	/*se termina para asegurar que el ppcb migre exitosamente o muera*/
		}
		
		Lista = PpcbAcr_Siguiente( Lista );
	}
	
	/*No hay Pendientes*/
}

/**********************************************************************/
void ACR_DeterminarNodo(tPpcbAcr* tPpcb)
{
	unsigned int 	alpe;
	tSocket* 		socket = NULL;
	t_nodo* 		lista_aux = ACR.t_ListaSocketAdp;
	unsigned char 	ip[4], ip2[4], ipIdeal[4], *pid;
	char 			* tmp, szIpAmplia[LEN_IP],	buffer [ PAQUETE_MAX_TAM ];
	int 			*iMaxMem, *iCantPcb, iCantPcbIdeal, len;
	float 			*fCargaProm, fCargaPromIdeal;
	unsigned short int *puerto, puertoIdeal, bNodoIdeal;
	tPaquete		* paq;
	
	bNodoIdeal = FALSE;
	fCargaPromIdeal = 99999999.99;/*Un valor maximo facilmente reemplazado*/
	
	ReducirIP(ACR.sz_ACR_IP,ip);
	
	while( lista_aux )
	{
		socket = nodo_datos( lista_aux, &alpe );
		
		tmp = paquetes_newPaqGetPerformanceAsStr(ip,_ID_ACR_,ACR.usi_ACR_Port);
		
		conexiones_sendBuff( socket, tmp, PAQUETE_MAX_TAM );

		len = conexiones_recvBuff(socket, buffer, PAQUETE_MAX_TAM);
	
		if ( ERROR == len || !len)
		{
			ACR_CerrarConexion( socket );		
			return;
		}
		
		paq = paquetes_CharToPaq( buffer );

		if( IS_PAQ_INFO_PERFORMANCE(paq) )
		{
			Log_log(log_debug, "Recibo PAQ_INFO_PERFORMANCE");
			paquetes_ParsearPaqInfoPerformance(buffer,ip2,pid,puerto,iMaxMem,fCargaProm,iCantPcb);
			
			if( *iCantPcb > 0 && *iMaxMem >= tPpcb->iMemoria ){
				Log_log(log_info,"Se tiene en cuenta info de performance");
				/* Si se permiten mas ppcb y el nodo tiene suficiente memoria */	
				if( *fCargaProm < fCargaPromIdeal ||
						(*fCargaProm = fCargaPromIdeal && *iCantPcb > iCantPcbIdeal ) ){
					Log_log(log_info,"Nuevos valores ideales, se cambia el nodo");
					fCargaPromIdeal = *fCargaProm;
					iCantPcbIdeal = *iCantPcb;
					memcpy(ipIdeal, ip2, 4);
					puertoIdeal = *puerto;
					bNodoIdeal = TRUE; 
				}
			}
			
		}
	
		if( paq )
			paquetes_destruir(paq);
		
		nodo_sgte( lista_aux );
		
	}
	
	bzero(szIpAmplia,LEN_IP);
	AmpliarIP(ipIdeal,szIpAmplia);
	
	if(bNodoIdeal){
		/* se encontro un nodo para migrar el ppcb */	
		Log_printf(log_info,"se encontro un nodo para migrar el ppcb id: %d, en IP: %s puerto %ud",
			tPpcb->pid,szIpAmplia,puertoIdeal);
		
		/*migrar PCB a nodo*/
		ACR_MigrarProceso(tPpcb,ipIdeal,puertoIdeal);
	}
	
}

/**********************************************************************/
void ACR_AtenderADS ( tSocket *sockIn )
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ];
	
	unsigned char IP[4],ip[4];
	unsigned char id_Proceso;
	unsigned short int puerto;
	char 		szNomProg[LEN_COMANDO_EJEC];
	char 		szUsuario[LEN_USUARIO];
	int			idSesion, pidChild;
	long		lpcb_id;

	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		ACR_CerrarConexion( sockIn );		
		return;
	}
	
	paq = paquetes_CharToPaq( buffer );

	if( IS_PAQ_EXEC_PROG(paq) ){
		Log_log(log_debug,"Me llega un paq EXEC_PROG");
		
		paquetes_ParsearPaqExecProg(buffer,IP,&id_Proceso,&puerto,szNomProg,szUsuario,&idSesion);
		/*Verificar si existe el programa*/
		
		ReducirIP(ACR.sz_ACR_IP,ip);
		
		if(ExistFile(ACR.sz_programPath,szNomProg)==0)
		{	/*existe el programa*/
			Log_printf(log_info,"Existe el programa %s en el directorio",szNomProg);
			
			/* Crear el ppcb */
			lpcb_id = ++lContProcesos;
			if ( (pidChild = ACR_ForkPPCBInicial( lpcb_id, szNomProg, szUsuario, idSesion )) != ERROR  &&
				ACR_CrearPPCBInicial( lpcb_id, pidChild, szNomProg, szUsuario, idSesion ) == OK ){
				Log_printf(log_info,"Se creo pid:%ld,prog:%s,usr:%s,sesion:%d",lpcb_id,szNomProg,szUsuario,idSesion);
				tmp = paquetes_newPaqProgExecutingAsStr(ip,_ID_ACR_,ACR.usi_ACR_Port,szNomProg,idSesion);
			}else{
				Log_printf(log_error,"Existe el programa %s en el directorio pero error creandolo",szNomProg);
				tmp = paquetes_newPaqNoProgAsStr(ip,_ID_ACR_,ACR.usi_ACR_Port,szNomProg,idSesion);		
			}
		}else
		{	/*no existe el programa*/
			Log_printf(log_info,"No existe el programa %s en el directorio",szNomProg);
			tmp = paquetes_newPaqNoProgAsStr(ip,_ID_ACR_,ACR.usi_ACR_Port,szNomProg,idSesion);
		}
		
		conexiones_sendBuff(sockIn,tmp,PAQUETE_MAX_TAM);
		Log_log(log_info,"Se envia respuesta el ADS");
	}
	
	if( paq )
		paquetes_destruir(paq);
}

/**********************************************************************/
void ACR_AtenderADP ( tSocket *sockIn )
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ]; 
	unsigned char	ip[4];

	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		ACR_CerrarConexion( sockIn );		
		return;
	}
		
	paq = paquetes_CharToPaq( buffer );

	if( paq )
		paquetes_destruir(paq);
}

/**********************************************************************/
void ACR_AtenderPPCB( tSocket *sockIn )
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ];
	char			szPathArch[15]; 
	long			lpcb_id;
	FILE*			arch;
	tPpcbAcr		*ppcbEncontrado;
	tPpcbAcr		ppcbAux;

	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		ACR_CerrarConexion( sockIn );		
		return;
	}
	
	paq = paquetes_CharToPaq( buffer );


	if ( IS_PAQ_MIGRAR( paq ) )
	{
		Log_log( log_debug, "Llega PAQ_MIGRAR" );
		
		memcpy( &lpcb_id, paq->msg, sizeof(long) );
		bzero(szPathArch,sizeof(szPathArch));
		
		ArmarPathPCBConfig( szPathArch, lpcb_id );
		
		
		
		if ( (arch = fopen( szPathArch, "wb+" )) )
		{
			sockIn->callback = &ACR_RecibirArchivo;
			sockIn->extra = (void*)lpcb_id;
			fclose(arch);
		}
		else
		{
			Log_logLastError( "No pude abrir el archivo de la migracion" );
		}
	}
	else if ( IS_PAQ_MIGRAR_OK( paq ) )
	{
		Log_log(log_debug,"Llega PAQ_MIGRAR_OK");
		
		if ( (ppcbEncontrado = PpcbAcr_ObtenerPpcbXSock(&ACR.t_ListaPpcbPend,&ppcbAux)) ){
			/*Migro exitosamente entonces deja de ser inactivo*/
			ppcbEncontrado->sActividad = Estado_Activo;
			ppcbEncontrado->sFechaInactvdad = time (NULL);
		}else{
			Log_log(log_warning,"Se recibio MIGRAR_OK pero no se encontro PPCB asociado para adminsitrar");
		}
	}

	if( paq )
		paquetes_destruir(paq);	
	
}

/**********************************************************************/
void ACR_RecibirArchivo( tSocket *sockIn )
{
	int 			len; 
	tPaqueteArch* 	paq ; 
	FILE*			arch;
	char 			*tmp;
	char			buffer [ PAQUETE_ARCH_MAX_TAM ];
	char			szPathArch[15];
	unsigned char	szIP[4];
	long			lpcb_id;
	int 			pidChild;

	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_ARCH_MAX_TAM );
	
	if ( ERROR == len || !len)
	{
		Log_log(log_error,"Se cierra socket en medio de migracion");
		ACR_CerrarConexion( sockIn );		
	
		return;
	}
	
	
	if( len != PAQUETE_ARCH_MAX_TAM )
	{ 
		Log_printf(log_warning,
			"Atencion no recibi el largo fijo de paquete. Me llega %d en vez de %d al transferir",
			len, PAQUETE_ARCH_MAX_TAM ); 
	}
	
	paq = paquetes_CharToPaqArch(buffer);

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
		}
	}
	else if ( IS_PAQ_FIN_MIGRAR( paq ) )
	{
		Log_log( log_debug, "llega un PAQ_FIN_MIGRAR" );
		lpcb_id = (long)sockIn->extra;
		sockIn->callback = 	&ACR_AtenderPPCB;
		
		/*Envio el msj de migracion OK*/
		Log_log( log_debug, "envio migrar ok" );
		ReducirIP( ACR.sz_ACR_IP, szIP );
		
		if ( (pidChild = ACR_ForkPPCB( lpcb_id )) != ERROR  &&
			ACR_CrearPPCB( lpcb_id, pidChild ) == OK ){
			
			if ( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqMigrarOKAsStr( szIP, _ID_ACR_, ACR.usi_ACR_Port ), 
					PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
			{
				Log_logLastError("enviando migrar_ok");
			}
			
		}else{
			if( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqMigrarFaultAsStr( szIP, _ID_ACR_, ACR.usi_ACR_Port ), 
					PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
			{
				Log_logLastError("enviando migrar_fault");
			}
		}
		
	}
	
	if ( paq ) 
		paquetes_Archdestruir( paq );
	
}
/**********************************************************/
void ACR_MigrarProceso( tPpcbAcr* tPpcb, unsigned char ipIdeal[4], unsigned short puertoIdeal)
{
	char 	*tmp;
	int		nSend;
	unsigned char	ip[4];
	
	ReducirIP(ACR.sz_ACR_IP,ip);
	
	tmp = paquetes_newPaqMigrateAsStr(ip,_ID_ACR_,ACR.usi_ACR_Port, ipIdeal, puertoIdeal);
		
	nSend = conexiones_sendBuff( tPpcb->socket, tmp, PAQUETE_MAX_TAM );
	
	if( nSend == (int)NULL || nSend == ERROR )
	{
		Log_printf(log_error,"Ocurrio un error en envío de PAQ_MIGRATE");
	}
}

/**********************************************************/
int ACR_ForkPPCB( long lpcbid )
{
	int pid;
	char szPCB_ID[10];
	
	sprintf( szPCB_ID, "%ld", lpcbid );
	
	Log_printf(log_debug,"Voy a hacer el fork del PCB");
	pid = fork();
    
    if( !pid )
    {
       	Log_printf(log_debug,"Voy a instanciar el PCB");
    	execl( "ppcb", szPCB_ID, NULL);
        Log_printf(log_debug,"Esto no deberia imprimirse -> FALLA en exec para instanciar PCB");
        Log_printf(log_debug,"Error en exec: %s", strerror(errno));
        
        return ERROR;
  	}
  	/*else if( pid )*/ 
	Log_printf(log_debug,"El PID del PCB es: %d ", pid );
	return pid;

}

/**********************************************************/
int ACR_ForkPPCBInicial( long lpcb_id, char szNomProg[LEN_COMANDO_EJEC], char szUsuario[LEN_USUARIO], int idSesion)
{
	int pid;
	char szPCB_ID[10], szSesionId[10], szPuerto[10];
	
	sprintf( szPCB_ID, "%ld", lpcb_id );
	sprintf( szSesionId, "%d", idSesion );
	sprintf( szPuerto, "%d", ACR.usi_ACR_Port);
	
	Log_printf(log_debug,"Voy a hacer el fork del PCB");
	pid = fork();
    
    if( !pid )
    {
       	Log_printf(log_debug,"Voy a instanciar el PCB");
    	execl( "ppcb", "ppcb", szPCB_ID, szUsuario, szNomProg, szSesionId, ACR.sz_ACR_IP, szPuerto,ACR.sz_programPath, NULL);
        Log_printf(log_debug,"Esto no deberia imprimirse -> FALLA en exec para instanciar PCB");
        Log_printf(log_debug,"Error en exec: %s", strerror(errno));
        
        return ERROR;
  	}
  	/*else if( pid )*/ 
	Log_printf(log_debug,"El PID del PCB es: %d ", pid );
	return pid;
}

/**********************************************************/
int	ACR_CrearPPCB( long lpcbid, int pidChild )
{
	char		szPathArch[15];
	tPpcbAcr	*ppcb =NULL;
	char 		*tmp;
	tConfig 	*cfg;
	
	if ( !( ppcb = malloc(sizeof(tPpcbAcr)) ) )
	{
		Log_logLastError("Creando datos de PPCB en ACR");
		return ERROR;
	}
	
	bzero(szPathArch,sizeof(szPathArch));
	ArmarPathPCBConfig( szPathArch, lpcbid );
	
	do
	{
		ppcb->pid = lpcbid;	/*PPCB ID*/
		ppcb->pidChild = pidChild;	/*process id*/
		
		/*Levanto la configuracion*/		
		if ( !(cfg = config_Crear( szPathArch, _PPCB_ )) ) 
			break;
		
		
		if ( (tmp = config_GetVal( cfg, _PPCB_, "CREATORID" ) ) )	/*USUARIO*/
		{
			strncpy( ppcb->szUsuario, tmp, LEN_USUARIO );
		}
		
		if ( (tmp = config_GetVal( cfg, _PPCB_, "COMANDO" ) ) )		/*COMANDO que lo EJECUTO*/
		{
			strncpy( ppcb->szComando, tmp, LEN_COMANDO_EJEC );
		}
		
		ppcb->lIdSesion = config_GetVal_Int( cfg, _PPCB_, "SESIONID" );		/*ID de SESION*/
		
		if ( (tmp = config_GetVal( cfg, _PPCB_, "CODE1" ) ) )		/*cantidad de MEMORIA*/
		{
			sscanf( tmp, "MEM %d", &(ppcb->iMemoria));
		}
		
		config_Destroy(cfg);

		ppcb->sActividad = Estado_Inactivo;
		/*ppcb->sFechaInactvdad = time(NULL);	Esto mejor hacerlo cuando el ppcb pida conexion*/
		ppcb->socket= NULL;   			/*nada ! Recien cuando se conecte desde mi nodo*/
		
		lista_insertar(&ACR.t_ListaPpcbPend, ppcb, sizeof(ppcb), &comparaPpcbAcr,_SIN_REPET_);
		
		
		return OK;
		
	}while(0);
	
	return ERROR;
	
}

/**********************************************************************/
int ACR_CrearPPCBInicial( long lpcb_id, int pidChild, char szNomProg[LEN_COMANDO_EJEC], char szUsuario[LEN_USUARIO], int idSesion)
{
	char		szPathArch[56+1];
	tPpcbAcr	*ppcb =NULL;
	FILE		*arch;
	
	if ( !( ppcb = malloc(sizeof(tPpcbAcr)) ) )
	{
		Log_logLastError("Creando datos de PPCB en ACR");
		return ERROR;
	}
	
	do
	{
		ppcb->pid = lpcb_id;	/*PPCB ID*/
		ppcb->pidChild = pidChild;	/*process id*/
		
		strncpy( ppcb->szUsuario, szUsuario, LEN_USUARIO );	/*USUARIO*/
		
		strncpy( ppcb->szComando, szNomProg, LEN_COMANDO_EJEC );	/*COMANDO que lo EJECUTO*/
				
		ppcb->lIdSesion = idSesion;		/*ID de SESION*/
		
		sprintf(szPathArch,"%s/%s",ACR.sz_programPath,szNomProg);
		
		if( !(arch = fopen( szPathArch, "r" )) ){
			Log_logLastError("Abriendo archivo emu");
			break;
		}
		fscanf( arch, "MEM %d", &(ppcb->iMemoria));	/*cantidad de MEMORIA*/
		fclose(arch);
		
		ppcb->sActividad = Estado_Inactivo;
		/*ppcb->sFechaInactvdad = time(NULL);	Esto mejor hacerlo cuando el ppcb pida conexion*/
		ppcb->socket=NULL;   			/*nada ! Recien cuando se conecte desde mi nodo*/
		
		lista_insertar(&ACR.t_ListaPpcbPend, ppcb, sizeof(ppcb), &comparaPpcbAcr,_SIN_REPET_);
		
		Log_printf(log_info,"Se creo el PPCB en ACR[id:%d %s idSesion:%d MEM:%d]",
					lpcb_id,szNomProg,idSesion,ppcb->iMemoria);
		return OK;
		
	}while(0);
	
	return ERROR;	
}

/**********************************************************************/
void ACR_CerrarConexion( tSocket *sockIn )
{
	conexiones_CerrarSocket( ACR.t_ListaSockets, sockIn, &ACR.ui_ultimoSocket );	
}

/**********************************************************************/
void ACR_DesconectarADS(tSocket *sockIn)
{
	Log_log(log_warning,"Se cierra Socket ADS");
}

/**********************************************************************/
void ACR_DesconectarADP(tSocket *sockIn)
{
	Log_log(log_warning,"Se cierra Socket ADP");
	lista_quitar(&ACR.t_ListaSocketAdp,sockIn, compararSocket);
	Log_log(log_debug,"Se elimina el socket de la lista de adps");
}

/**********************************************************************/
void ACR_DesconectarPPCB(tSocket *sockIn)
{
	Log_log(log_warning,"Se cierra Socket PPCB");
}
