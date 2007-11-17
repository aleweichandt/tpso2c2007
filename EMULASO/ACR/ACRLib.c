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
		DatosAdpACR_Ini( &ACR.t_ListaSocketAdp );
		
		ACR.t_ListaPpcbPend = NULL;
		lista_inic( &ACR.t_ListaPpcbPend );
		
		/*signals*/
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
	
	DatosAdpACR_EliminarLista( &ACR.t_ListaSocketAdp );
	
	conexiones_CerrarLista( 0, &ACR.ui_ultimoSocket, ACR.t_ListaSockets );
	free(ACR.t_ListaSockets);
	
	Log_Cerrar();
	/*pantalla_Clear();*/
	exit(EXIT_SUCCESS);
}

/**********************************************************************/
int ACR_LeerConfig()
{
	char 	*tmp;
	tConfig *cfg;
	int 	tmpInt;
	
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
		/*Log_printf(log_debug,"LIFE_TIME_PPCB:  %d",ACR.i_maxLifeTimePPCB);*/
		
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
		
		ACR.MatrizAsignacion = NULL;
		MatrizRec_Iniciar(&ACR.MatrizAsignacion,ACR.nCantRecursos);
		
		Rec_InicializarLista(ACR.ListaRecursos,&ACR.nCantRecursos);
		
		tmpInt = config_GetVal_Int( cfg, _ACR_, "IMPRESORA" );
		Rec_Agregar(ACR.ListaRecursos,"Impresora",tmpInt,&ACR.nCantRecursos);
		
		tmpInt = config_GetVal_Int( cfg, _ACR_, "DISCO" );
		Rec_Agregar(ACR.ListaRecursos,"Disco",tmpInt,&ACR.nCantRecursos);
		
		tmpInt = config_GetVal_Int( cfg, _ACR_, "CINTA" );
		Rec_Agregar(ACR.ListaRecursos,"Cinta",tmpInt,&ACR.nCantRecursos);
		
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
{
	int nstateChld;
 	
 	if ( senial == SIGUSR1 )
	{	/* TODO: Escribir info de estado del sistema */
		Log_log( log_info, "Recibo senial SIGUSR1");		
		ACR_ImprimirInfoCtr();
		signal( SIGUSR1, ACR_ProcesarSeniales );
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

void ACR_Timer( tSocket* sockIn )
{
	/*Log_log( log_info, "Recibo senial SIGALRM");*/
	
	ACR_ControlarPendientes();
	ACR_ControlarConcesionRecursos();
}

/**********************************************************************/
void ACR_PonerTimer(time_t* tiempo){
	int alarma = ALRM_T;
	time_t now = time(NULL);
	if( difftime( now, *tiempo) < ALRM_T ){
		alarma = ALRM_T - difftime (now, *tiempo);
	}else{
		*tiempo = now;
	}
	
	ACR.t_ListaSockets[ SOCK_ESCUCHA ]->onTimeOut= ACR_Timer;
	ACR.t_ListaSockets[ SOCK_ESCUCHA ]->segundos_timeout = alarma;
}

/**********************************************************/
void ACR_ImprimirInfoCtr()
{
	int i, pos;
	long ppcbid;
	tDatosRecurso* recurso;
	
	Log_log(log_info, "  Se imprime la informacion de control del ACR  ");
	
	InfoCtr_Inicializar("ACR","1");
	
	for (i = 0; i < ACR.nCantRecursos; ++i) {

		recurso = Rec_BuscarXPos(ACR.ListaRecursos, ACR.nCantRecursos, i);
		InfoCtr_printf(log_info, "recurso:  %s tiene %d disponibles de un total de %d (sem:%d).",
				recurso->szNombre,recurso->nAvailable,recurso->nInstancias, recurso->nSemaforo );

		InfoCtr_log(log_info,"Los PPCBs que lo estan usando son: ");
		ACR_ImprimirPpcbUsando(&ACR.MatrizAsignacion,i);

		InfoCtr_log(log_info,"Los PPCBs que lo estan esperando son: ");
		/*TODO: Hacer*/
		pos = 0;
		while( (ppcbid = Rec_ObtenerBloqueado(recurso, pos)) > 0){
			InfoCtr_printf(log_info,"%ld",ppcbid);
			pos++;
		}
	}
	
	InfoCtr_CerrarInfo();

}

/**********************************************************************/
void ACR_ImprimirPpcbUsando(tListaFilas *matriz, int posRecurso)
{
	int i;
	int nResul;
	tPpcbAcr* ppcb;
	tListaPpcbAcr ListaPpcb = ACR.t_ListaPpcbPend;

	while(ListaPpcb)
	{
		ppcb = PpcbAcr_Datos(ListaPpcb);
		nResul = MatrizRec_ObtenerInstancia(matriz,ACR.nCantRecursos, ppcb->pid, posRecurso);
		/*Log_printf(log_debug,"resultado Instancia: %d",nResul);*/
		if(nResul == ERROR)
		{
			Log_printf(log_warning,"No se encuentra la fila P:%ld en la matriz",ppcb->pid);
		}else if( nResul > 0 )
		{
			InfoCtr_printf(log_info, "	ppcb-id: %ld, prog %s de %s. (instancias:%d)", ppcb->pid,ppcb->szComando,ppcb->szUsuario,nResul);
			/*InfoCtr_printf(log_info, "---> %s.", transferencia->nombreTrans);*/
		}
		ListaPpcb = PpcbAcr_Siguiente(ListaPpcb);
	}
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
	int 			len, pos;
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ]; 
	unsigned char 	szIP[4];
	int 			nSend, bSendPong = FALSE;
	long			ppcbid;
	tPpcbAcr*		ppcb;
	
	
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
		
		DatosAdpACR_Agregar( &ACR.t_ListaSocketAdp, sockIn );
		
		bSendPong = TRUE;
	}
	else if(IS_PPCB_PAQ(paq) && IS_PAQ_PING( paq ) )
	{/*Bienvenido PPCB*/
		
		Log_log( log_debug, "PPCB me manda un ping para establecer conexion" );
		
		sockIn->callback = &ACR_AtenderPPCB;
		sockIn->onClose = &ACR_DesconectarPPCB;
		sockIn->estado = estadoConn_escuchando;
		
		/*Determina a que proceso pertenece para asociar el socket*/
		memcpy(&ppcbid,paq->msg,sizeof(long));
		Log_printf(log_info,"Proceso PPCB id:%ld se identifica con socket",
						ppcbid);
		if( (ppcb = PpcbAcr_BuscarPpcb(&ACR.t_ListaPpcbPend,ppcbid,&pos)) == NULL )
		{
			Log_log(log_warning,"El Proceso que se conecto no es de los que administro, cierro conexion");
			ACR_CerrarConexion(sockIn);
			bSendPong = FALSE;
		} else {
			ppcb->socket = sockIn;
			ppcb->sFechaInactvdad = time(NULL);	/*Seteo la fecha de inicio en el sistema para timeout*/
			bSendPong = TRUE;
		}
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
		
		/*Log_printf(log_debug,"difftime de ppcb %ld es: %.2f, %d %d %d ",
					ppcb->pid, difftime( now, ppcb->sFechaInactvdad ),
					ppcb->sActividad, Estado_Inactivo,
					ppcb->sActividad == Estado_Inactivo
					); /* Lo saco porque esta funcionando */
		if ( (ppcb->sActividad == Estado_Inactivo)&&
				difftime( now, ppcb->sFechaInactvdad ) > (double)ACR.i_maxLifeTimePPCB )
		{
			Log_printf(log_info,
				"Se elimina PPCB id:%ld %s de %s, de LPendientes por timeout(%.2fseg pasaron)",
				ppcb->pid, ppcb->szComando, ppcb->szUsuario, difftime( now, ppcb->sFechaInactvdad ));
			
			/*Elimino el proceso PPCB*/
			kill( ppcb->pidChild, SIGTERM );
			PpcbAcr_EliminarPpcb( &ACR.t_ListaPpcbPend, ppcb->pid );
			Lista = ACR.t_ListaPpcbPend;
			continue;
			
		}else if ( ppcb->sActividad == Estado_Inactivo )
		{
			Log_printf(log_info,
				"Intento reubicar PPCB id:%ld %s de %s, en nodo de ejecucion",
				ppcb->pid, ppcb->szComando, ppcb->szUsuario);
			
			ppcb->sActividad = Estado_Neutral;
			ACR_DeterminarNodo(ppcb);
			break;	/*se termina para asegurar que el ppcb migre exitosamente o muera*/
			/*para probar*/
			
		}else if ( ppcb->sActividad == Estado_Neutral )
		{
			break;	/*se espera para ver el resultado del procedimiento de migracion*/
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
	tSocket*		socketIdeal = NULL;
	unsigned char 	ip[4], ip2[4], ipIdeal[4], pid;
	char 			* tmp, szIpAmplia[LEN_IP],	buffer [ PAQUETE_MAX_TAM ];
	int 			iMaxMem, iCantPcb, iCantPcbIdeal, len, indice=0, nSend;
	float 			fCargaProm, fCargaPromIdeal;
	unsigned short int puerto, puertoIdeal, bNodoIdeal;
	tPaquete		* paq;
	
	bNodoIdeal = FALSE;
	fCargaPromIdeal = 99999999.99;/*Un valor maximo facilmente reemplazado*/
	
	ReducirIP(ACR.sz_ACR_IP,ip);
	
	while( socket = DatosAdpACR_Obtener( &ACR.t_ListaSocketAdp, indice ) )
	{
		Log_log(log_debug, "Tengo un adp candidato");
		tmp = paquetes_newPaqGetPerformanceAsStr(ip,_ID_ACR_,ACR.usi_ACR_Port);
		
		nSend = conexiones_sendBuff( socket, tmp, PAQUETE_MAX_TAM );
		if( nSend == (int)NULL || nSend == ERROR )
			Log_logLastError("Ocurrio un error en envío de PAQ_GET_PERFORMANCE");
		
		len = conexiones_recvBuff(socket, buffer, PAQUETE_MAX_TAM);
	
		if ( ERROR == len || !len)
		{
			tPpcb->sActividad = Estado_Inactivo;
			ACR_CerrarConexion( socket );		
			return;
		}
		
		paq = paquetes_CharToPaq( buffer );

		if( IS_PAQ_INFO_PERFORMANCE(paq) )
		{
			Log_log(log_debug, "Recibo PAQ_INFO_PERFORMANCE");
			paquetes_ParsearPaqInfoPerformance(buffer,ip2,&pid,&puerto,&iMaxMem,&fCargaProm,&iCantPcb);
			Log_printf(log_debug,"iMaxMem=%d,fCargaProm=%.2f,iCantPcb=%d,condicional=%d",iMaxMem,fCargaProm,iCantPcb,
						iCantPcb > 0 && iMaxMem >= tPpcb->iMemoria);
			if( iCantPcb > 0 && iMaxMem >= tPpcb->iMemoria ){
				Log_log(log_info,"Se tiene en cuenta info de performance");
				/* Si se permiten mas ppcb y el nodo tiene suficiente memoria */	
				if( fCargaProm < fCargaPromIdeal ||
						(fCargaProm == fCargaPromIdeal && iCantPcb > iCantPcbIdeal ) ){
					Log_log(log_info,"Nuevos valores ideales, se cambia el nodo");
					fCargaPromIdeal = fCargaProm;
					iCantPcbIdeal = iCantPcb;
					memcpy(ipIdeal, ip2, sizeof(unsigned char[4]));
					puertoIdeal = puerto;
					socketIdeal = socket;
					bNodoIdeal = TRUE;
				}
			}
			
		}else{
			Log_log(log_warning,"El ADP me responde algo que no es un PAQ_INFO_PERFORMANCE");
		}
	
		if( paq )
			paquetes_destruir(paq);
		
		indice++;
		
	}
	
	bzero(szIpAmplia,LEN_IP);
	AmpliarIP(ipIdeal,szIpAmplia);
	
	if(bNodoIdeal){
		/* se encontro un nodo para migrar el ppcb */	
		Log_printf(log_info,"se encontro un nodo para migrar el ppcb id: %d, en IP: %s puerto %d",
			tPpcb->pid,szIpAmplia,puertoIdeal);
		
		tPpcb->socketADP = socketIdeal;		/*se agrega el socket del adp*/
		/*migrar PCB a nodo*/
		ACR_MigrarProceso(tPpcb,ipIdeal,puertoIdeal);
	}else{
		tPpcb->sActividad = Estado_Inactivo;
	}
	
}

/**********************************************************************/
void ACR_AtenderADS ( tSocket *sockIn )
{
	int 			len; 
	tPaquete* 		paq = NULL ; 
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
	else if( IS_PAQ_END_SESION(paq) ){
		tPaquete* paqSend = NULL;
		int nSend;
		char *stringSesion;
		
		ReducirIP(ACR.sz_ACR_IP,ip);
		idSesion=atoi(paq->msg);
		Log_log(log_debug,"me llega un paq END_SESION");
		
		if(ACR_LiberarRecursos(idSesion)==OK){
			Log_printf(log_info,"Se liberaron los recursos de la sesion %i con exito",idSesion);
			stringSesion=malloc(sizeof(int)+1);
			sprintf(stringSesion,"%i",idSesion);
			if ( !(paqSend  = paquetes_newPaqEnd_Sesion_Ok( ip, _ID_ACR_,ACR.usi_ACR_Port, paq->msg )) )
			{
				Log_log( log_error, "Error creando paquete End_Sesion" );
			}
			nSend = conexiones_sendBuff( sockIn, (const char*)paquetes_PaqToChar( paqSend ), PAQUETE_MAX_TAM );
			if ( nSend != PAQUETE_MAX_TAM )
			{
				Log_log( log_error,"Error enviando end_sesion_ok al ADS" );
			}
		}else{
			Log_printf(log_error,"No se han liberado los recursos de la sesion %i",idSesion);
		}
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
	unsigned char 	id_Proceso;
	unsigned short 	puerto;
	tRecurso		recurso;
	int				ppcbid;

	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		ACR_CerrarConexion( sockIn );		
		return;
	}
		
	paq = paquetes_CharToPaq( buffer );
	
	if(IS_PAQ_PRINT(paq))
	{/*me llega el print*/
		int nSend;
		long int pid;
		/*chequeo el unused en la cabecera por si finaliza el pcb*/
		memcpy(&pid, &(paq->id.UnUsed), sizeof(long int) );
		if(pid != 0){
			/*esta cargado el unused, se liberan los recursos*/
			Log_printf(log_debug,"detecto finalizacion por paquete print. Se liberan los recursosdel PPCB id:%i",pid);
				
			ACR_DevolverTodos(pid);
		}
		
		/*se lo reenvio al ADS*/
		Log_log( log_debug, "Mando PRINT al ADS" );
		nSend = conexiones_sendBuff( ACR.psocketADS, (const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
					
		if(nSend != PAQUETE_MAX_TAM)
		{
			Log_logLastError("error al enviar PRINT al ADS" );
		}
	}
	if(IS_PAQ_DEV(paq))
	{/*me llega un DEV del ADP. libero el recurso*/
		int id;
		tRecurso rec;
		
		Log_log(log_debug, "me llego un DEV del ADP");
		
		memcpy( &(paq->msg[SOLDEV_POS_PPCBID]), &id ,  sizeof (int) );
		memcpy( &(paq->msg[SOLDEV_POS_RECURSO]), &rec , sizeof (tRecurso));
		
		if(ACR_DevolverRecurso(id,rec)==OK)Log_log(log_info,"se devolvio el recurso correctamente");
		else Log_log(log_info,"no hay recursos que devolver");
	}
	else if ( IS_PAQ_INFO_PCBS_STATES( paq ) )
	{
/*	Gonza: aca tenes como recorrer el paquete, si queres hace las conversiones	
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
*/		
	}
	else if( IS_PAQ_SOL(paq) ){
		
		paquetes_ParsearSol(buffer,ip,&id_Proceso,&puerto,&ppcbid,&recurso);
		
		ACR_PedirRecurso((long)ppcbid, recurso);
	}
	
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
		
		ppcbAux.socket = sockIn;
		if ( (ppcbEncontrado = PpcbAcr_ObtenerPpcbXSock(&ACR.t_ListaPpcbPend,&ppcbAux)) ){
			/*Migro exitosamente entonces deja de ser inactivo*/
			ppcbEncontrado->sActividad = Estado_Activo;
			ppcbEncontrado->sFechaInactvdad = time (NULL);
			/*al migrar bien mato al proceso original*/
			Log_printf(log_debug,"Se mata a proceso pid: %d",ppcbEncontrado->pidChild);
			if(kill( ppcbEncontrado->pidChild, SIGTERM ) == ERROR){
				Log_logLastError("kill(SIGTERM) al proceso hijo");
			}
		}else{
			Log_log(log_warning,"Se recibio MIGRAR_OK pero no se encontro PPCB asociado para adminsitrar");
		}
	}
	else if( IS_PAQ_MIGRAR_FAULT(paq) )
	{
		Log_log(log_debug,"Llega PAQ_MIGRAR_FAULT");
		
		ppcbAux.socket = sockIn;
		if ( (ppcbEncontrado = PpcbAcr_ObtenerPpcbXSock(&ACR.t_ListaPpcbPend,&ppcbAux)) ){
			/*Migro exitosamente entonces deja de ser inactivo*/
			ppcbEncontrado->sActividad = Estado_Inactivo;
			ppcbEncontrado->socketADP = NULL;
			/*ppcbEncontrado->sFechaInactvdad = time (NULL);  Esto no, se continua con el primer timer*/
		}else{
			Log_log(log_warning,"Se recibio MIGRAR_FAULT pero no se encontro PPCB asociado para adminsitrar");
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
			Log_printf(log_info,"Se creo PCB pid:%ld",lpcb_id);
			if ( conexiones_sendBuff( sockIn, (const char*) paquetes_newPaqMigrarOKAsStr( szIP, _ID_ACR_, ACR.usi_ACR_Port ), 
					PAQUETE_MAX_TAM ) != PAQUETE_MAX_TAM )
			{
				Log_logLastError("enviando migrar_ok");
			}
			
		}else{
			Log_printf(log_error,"No se creo PCB pid:%ld",lpcb_id);
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
	char szPCB_ID[10], szIdProceso[10];
	
	sprintf( szPCB_ID, "%ld", lpcbid );
	sprintf( szIdProceso, "%d", _ID_ACR_ );
	
	Log_printf(log_debug,"Voy a hacer el fork del PCB");
	pid = fork();
    
    if( !pid )
    {
       	Log_printf(log_debug,"Voy a instanciar el PCB");
    	execl( "ppcb", "ppcb", szPCB_ID, szIdProceso, NULL);
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
	char		szPathArch[51];
	tPpcbAcr	*ppcb =NULL;
	tPpcbAcr	ppcbAux;
	char 		*tmp;
	int 		pos;
	tConfig 	*cfg;
	
/*	if ( !( ppcb = malloc(sizeof(tPpcbAcr)) ) )
	{
		Log_logLastError("Creando datos de PPCB en ACR");
		return ERROR;
	}
*/	
	bzero(szPathArch,sizeof(szPathArch));
	ArmarPathPCBConfig( szPathArch, lpcbid );
	
	do
	{
	/*	ppcb->pid = lpcbid;	/*PPCB ID*/
		ppcbAux.pid = lpcbid;
		if( (ppcb = PpcbAcr_ObtenerPpcbXPid(&ACR.t_ListaPpcbPend,&ppcbAux)) == NULL )
		{
			Log_log(log_warning,"El Proceso que migro no es de los que administro, lo mato");
			kill(pidChild, SIGTERM);
			break;
		}
		
		Log_printf(log_debug,"Encuentro el ppcb que migro dentro de los que administro. pid:%ld",lpcbid);		
		
		/*Levanto la configuracion*/		
/*		if ( !(cfg = config_Crear( szPathArch, _PPCB_ )) ) 
			break;
		
		
		if ( (tmp = config_GetVal( cfg, _PPCB_, "ESTADO" ) ) )	/*ESTADO*/
/*		{
			if (!strcmp("PENDIENTE", config_GetVal( cfg, _PPCB_, "ESTADO")) ){
				ppcb->State = PENDIENTE;
			} else if (!strcmp("BLOQUEADO", config_GetVal( cfg, _PPCB_, "ESTADO")) ) {
			PCB.State = BLOQUEADO;
		} else if (!strcmp("LISTO", config_GetVal( cfg, _PPCB_, "ESTADO")) ) {
			PCB.State = LISTO;
		} else if (!strcmp("EJECUTANDO", config_GetVal( cfg, _PPCB_, "ESTADO")) ) {
			PCB.State = EJECUTANDO;
		}
			strncpy( ppcb->szUsuario, tmp, LEN_USUARIO );
		}
		
/*		if ( (tmp = config_GetVal( cfg, _PPCB_, "COMANDO" ) ) )		/*COMANDO que lo EJECUTO*/
/*		{
			strncpy( ppcb->szComando, tmp, LEN_COMANDO_EJEC );
		}
		
		ppcb->lIdSesion = config_GetVal_Int( cfg, _PPCB_, "SESIONID" );		/*ID de SESION*/
		
/*		if ( (tmp = config_GetVal( cfg, _PPCB_, "CODE1" ) ) )		/*cantidad de MEMORIA*/
/*		{
			sscanf( tmp, "MEM %d", &(ppcb->iMemoria));
		}
		
		config_Destroy(cfg);

		PpcbAcr_AgregarPpcb(&ACR.t_ListaPpcbPend, ppcb );
*/
		ppcb->pidChild = pidChild;	/*process id*/
		ppcb->sActividad = Estado_Inactivo;
		ppcb->sFechaInactvdad = time(NULL);	/*Esto mejor hacerlo cuando el ppcb pida conexion, pero lo hago = porque evita error en ControlarPendientes*/
		ppcb->socket= NULL;   			/*nada ! Recien cuando se conecte desde mi nodo*/
		ppcb->socketADP = NULL;
		
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
		ppcb->sFechaInactvdad = time(NULL);	/*Esto mejor hacerlo cuando el ppcb pida conexion*/
		ppcb->socket=NULL;   			/*nada ! Recien cuando se conecte desde mi nodo*/
		ppcb->socketADP = NULL;
		
		if( !PpcbAcr_AgregarPpcb(&ACR.t_ListaPpcbPend, ppcb ))
		{
			Log_printf(log_error,"insertando ppcb a la lista de pendientes");
			break;
		}
		
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
	DatosAdpACR_EliminarDeLaLista(&ACR.t_ListaSocketAdp,sockIn);
	Log_log(log_debug,"Se elimina el socket de la lista de adps");
}

/**********************************************************************/
void ACR_DesconectarPPCB(tSocket *sockIn)
{
	Log_log(log_warning,"Se cierra Socket PPCB");
}
/**********************************************************************/
int ACR_LiberarRecursos(int idSesion){
	tListaPpcbAcr 	Lista = ACR.t_ListaPpcbPend;
	tPpcbAcr		*ppcb = NULL;
	time_t			now = time(NULL);
	unsigned int 	alpe;
	tSocket* 		socket = NULL;
	unsigned char 	ip[4];
	tPaquete* paqSend=NULL;
	int nSend, pidVector[25], i = 0, indice = 0;
	
	
	ReducirIP(ACR.sz_ACR_IP,ip);
	/*memset(pidVector, -1, 25*sizeof(int) );*/
	for(i=0;i<25;i++)pidVector[i]=0;
	i=0;
	
	Log_log(log_debug,"se procede a eliminar los ppcbs no migrados");
	while( Lista )
	{
		ppcb = PpcbAcr_Datos( Lista );
		if(ppcb != NULL){		
			if( ppcb->lIdSesion == idSesion){
				if ( ppcb->sActividad != Estado_Activo  )
				{
					Log_printf(log_info,
						"Se elimina PPCB id:%ld de LPendientes por logout",
						ppcb->pid);
				
					/*Elimino el proceso PPCB*/
					ACR_DevolverTodos(ppcb->pid);
					kill( ppcb->pidChild, SIGTERM );
					PpcbAcr_EliminarPpcb( &ACR.t_ListaPpcbPend, ppcb->pid );
					Lista = ACR.t_ListaPpcbPend;
					
				}
				else if(ppcb->sActividad == Estado_Activo)
				{
					/*si esta activo acumulo en vector de pids para mandar a adps*/
					pidVector[i]=ppcb->pid;
					i++;
					ACR_DevolverTodos(ppcb->pid);
					PpcbAcr_EliminarPpcb( &ACR.t_ListaPpcbPend, ppcb->pid );
					Lista = ACR.t_ListaPpcbPend;
				}else{
					Lista = PpcbAcr_Siguiente( Lista );
				}
			}
		}
	}
	if(pidVector[0]!=0){
		Log_log(log_debug,"se envia end_sesion a cada adp");
		paqSend = paquetes_newPaqKill(ip,_ID_ACR_,ACR.usi_ACR_Port,pidVector);
		while( socket = DatosAdpACR_Obtener( &ACR.t_ListaSocketAdp, indice ) )
		{
			
			nSend = conexiones_sendBuff( socket, (const char*)paquetes_PaqToChar( paqSend ), PAQUETE_MAX_TAM );
			if ( nSend != PAQUETE_MAX_TAM )
			{
				Log_log( log_error,"Error enviando end_sesion al ADP" );
			}
			indice++;
		}
	}
	return OK;
}
/************************************************************************/
int ACR_DevolverTodos(int id){
	int *valores;
	int i;
	
	valores = MatrizRec_ObtenerVectorInstancia(&ACR.MatrizAsignacion,ACR.nCantRecursos,(long)id);
	if(valores == NULL)return ERROR;
	
	for(i=0;i<MAX_LISTA_REC;i++){
		ACR.ListaRecursos[i].nAvailable+=valores[i];
		ACR.ListaRecursos[i].nSemaforo+=valores[i];
		if(ACR.ListaRecursos[i].nSemaforo > 0)ACR.ListaRecursos[i].nAvailable=ACR.ListaRecursos[i].nSemaforo;
		
		Log_printf(log_info,"estado de recurso %i: semaforo=%i,disponibles=%i",i,ACR.ListaRecursos[i].nSemaforo, ACR.ListaRecursos[i].nAvailable);
	}
	return (MatrizRec_EliminarProceso(&ACR.MatrizAsignacion,ACR.nCantRecursos,(long)id));
}
/*************************************************************************/
int ACR_DevolverRecurso(int id,tRecurso rec){
	
	int *valores;
	valores = MatrizRec_ObtenerVectorInstancia(&ACR.MatrizAsignacion,ACR.nCantRecursos,(long)id);
	if(valores == NULL)return ERROR;
		
	ACR.ListaRecursos[rec].nSemaforo++;
	if(ACR.ListaRecursos[rec].nSemaforo > 0)ACR.ListaRecursos[rec].nAvailable++;
	
	Log_printf(log_info,"estado de recurso %i: semaforo=%i,disponibles=%i",rec,ACR.ListaRecursos[rec].nSemaforo, ACR.ListaRecursos[rec].nAvailable);
	
	return (MatrizRec_RestarInstancia(&ACR.MatrizAsignacion,ACR.nCantRecursos,(long)id,	rec, 1));
}
/**********************************************************************/
void ACR_PedirRecurso(long ppcbid, tRecurso recurso)
{
	int pos;
	tPpcbAcr* ppcb;
	
	do{
		/* Verifica si el usuario tiene permiso sobre el recurso pedido*/
		if( (ppcb = PpcbAcr_BuscarPpcb(&ACR.t_ListaPpcbPend,ppcbid,&pos)) == NULL ){
			Log_printf(log_error,"no se ha encontrado el ppcb id: %ld",ppcbid);
			break;
		}
		
		if( !ACR_VerificaPermisosUsuario(ppcb->szUsuario,recurso) ){
			Log_printf(log_info,"El usuario %s no tiene permisos sobre el recurso %d",
						ppcb->szUsuario,recurso);
			break;
		}
		
		Rec_DecrementarInst(&ACR.ListaRecursos[recurso],FALSE);
		/*no asigna pero disminuye el semaforo*/
		
		if( Rec_AgregarBloqueado(&ACR.ListaRecursos[recurso], ppcbid) == ERROR ){
			Log_log(log_error,"no se pudo agregar a la lista de bloqueados");
			break;
		}
		
		Log_printf(log_info,"el ppcb_id %ld ha pedido recurso %d",ppcbid,recurso);
		return;	
	}while(0);
	Log_printf(log_error,"el ppcb_id %ld no pudo pedir recurso %d",ppcbid,recurso);
	
}

/**********************************************************************/
int ACR_VerificaPermisosUsuario(char szUsuario[LEN_USUARIO], tRecurso recurso){
	char szLinea[LEN_MAX_LINEA_ARCH_USUARIOS] = {'\0'};
	char szUserNameExt[LEN_USUARIO] = {'\0'};
	char szRecursosExt[LEN_MAX_LINEA_ARCH_USUARIOS] = {'\0'};
	FILE *pfile;
	
	if( ( pfile = fopen( ACR.sz_userPath, "r" ) ) == NULL )
	{
		Log_printf(log_debug, "Error al abrir el archivo de Usuarios\n");
		return FALSE;
	}
	
	while(fgets(szLinea,LEN_MAX_LINEA_ARCH_USUARIOS, pfile))
	{
		ACR_ExtraerUserName(szUserNameExt, szLinea);
		ACR_ExtraerRecursos(szRecursosExt, szLinea);
		if ((strcmp(szUsuario, szUserNameExt) == 0) && 
				ACR_EstaRecurso(ACR.ListaRecursos[recurso].szNombre, szRecursosExt))
		{
			fclose(pfile);
			return TRUE;
		}
	}
	fclose(pfile);
	return FALSE;
}
/**********************************************************/
void ACR_ExtraerUserName(char* szUserNAme, const char* szLinea)
{
	char szLineaCpy[LEN_MAX_LINEA_ARCH_USUARIOS] = {'\0'};
	
	strcpy(szLineaCpy, szLinea);
	strcpy(szUserNAme, strtok(szLineaCpy, ":"));
	return;
}
/**********************************************************/
void ACR_ExtraerRecursos(char* szRecursos, const char* szLinea)
{
	char szLineaCpy[LEN_MAX_LINEA_ARCH_USUARIOS] = {'\0'};
	char* p;
	strcpy(szLineaCpy,szLinea);
	p = strtok(szLineaCpy, ":");
	p = strtok(NULL, ":");
	p = strtok(NULL, ":");
	strcpy(szRecursos, p);
	return;
}

/***********************************************************************************/
char ACR_EstaRecurso( const char* szRecursoPedido, char *szRecursos )
{
	char szEncrips[256];
	char *pAux;
	
	if ( strlen(szRecursos) == 0 )
		return FALSE;
	
	strcpy( szEncrips, szRecursos );/*Hago esto porque el strtok afecta al char de entrada*/
	
	pAux = strtok( szEncrips, _SEP_RECURSO_ );
	
	while ( pAux )
	{
		if ( strcmp( pAux, szRecursoPedido ) == 0 )
		{
			return TRUE;			
		}
		
		pAux = strtok( NULL, _SEP_RECURSO_ );
	}
	return FALSE;
}

/**********************************************************************/
void ACR_ControlarConcesionRecursos()
{
	int i;
	tDatosRecurso* recurso;
	
	for (i = 0; i < ACR.nCantRecursos; ++i) {

		recurso = Rec_BuscarXPos(ACR.ListaRecursos, ACR.nCantRecursos, i);
		ACR_ControlarConcesionRecurso(recurso,i);
	}
}

/**********************************************************************/
void ACR_ControlarConcesionRecurso(tDatosRecurso* recurso, int posRecurso)
{
	int 	pos;
	long 	ppcbid;
	tPpcbAcr* ppcb = NULL;
	
	/* se verifica si el recurso tiene ppcbs bloqueados */
	if( Rec_SinBloqueados(recurso) )
		return;
	
	/* se verifica que hayan instancias disponibles del recurso */
	if(recurso->nAvailable == 0)
		return;
	
	ppcbid = Rec_ObtenerBloqueado(recurso,0);

	if( ppcbid == (long)NULL ){
		Log_log(log_warning,"se obtuvo un bloqueado nulo");
		return;
	}
	
	if( (ppcb = PpcbAcr_BuscarPpcb(&ACR.t_ListaPpcbPend,ppcbid,&pos)) == NULL ){
		Log_printf(log_error,"no se ha encontrado el ppcb id: %ld",ppcbid);
		return;
	}
	
	if( ppcb->sActividad != Estado_Activo ){
		Log_printf(log_info,"el ppcb id %ld que se quizo asignar no está en un nodo de red de carga",
					ppcb->pid);
		return;
	}
	
	ACR_ConcederRecurso( ppcb, recurso, posRecurso );
	
}

/**********************************************************************/
void ACR_ConcederRecurso(tPpcbAcr* ppcb, tDatosRecurso* recurso, int posRecurso)
/* Esta funcion tiene efecto de lado, que se evito
 * tener anteriormente en el procedimiento */
{
	int 	nSend;
	long 	ppcbidBLoqueado = Rec_QuitarBloqueado(recurso);
	char 	*tmp;
	unsigned char ip[4]={'\0','\0','\0','\0'};
	
	if( ppcbidBLoqueado != ppcb->pid ){
		Log_printf(log_error, "el ppcbid que se saco de la lista de bloqueados (%ld) no coincide con el ppcb(%ld)",
					ppcbidBLoqueado, ppcb->pid );
	}
	
	if( Rec_DecrementarInst(recurso,TRUE) ){
		Log_printf( log_info, "Se concedio el recurso %s al ppcb id %ld %s de %s",
				 	recurso->szNombre, ppcb->pid, ppcb->szComando, ppcb->szUsuario );
		
		MatrizRec_SumarInstancia(&ACR.MatrizAsignacion,ACR.nCantRecursos,ppcb->pid, posRecurso,1);
		
		tmp = paquetes_newPaqSolConcedidoAsStr(ip,_ID_ACR_,ACR.usi_ACR_Port,(int)ppcb->pid);
		
		nSend = conexiones_sendBuff( ppcb->socketADP, tmp, PAQUETE_MAX_TAM );
		if( nSend == (int)NULL || nSend == ERROR )
			Log_logLastError("Ocurrio un error en envío de PAQ_SOL_CONCEDIDO");
		
		
	}else{
		Log_printf( log_error, "No se concedio el recurso %s al ppcb id %ld %s de %s",
				 	recurso->szNombre, ppcb->pid, ppcb->szComando, ppcb->szUsuario );
	}
	
}
