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
		
		if ( ADS_ConectarACR() == ERROR )
		{
			Log_log( log_error, "Error conectandose con el ACR!" );
			/*break; TODO: descomentar cuando pueda conectarme al ACR*/
		}
		
		signal(SIGALRM, ADS_ProcesarSeniales);
		signal(SIGCHLD, ADS_ProcesarSeniales);
		/*tests*/
		if(0)
		{
			char  str[LEN_USERNAME] = {'\0'};
			char  str2[LEN_USERNAME] = {'\0'};
			int i;
			tUsuarioADS * usr;
			strcpy(str2,ADS_BuscarUsuario("/home/miguel/emulaso/usuarios", "miguel"));
			UsuariosADS_AgregarUsr(&ADS.m_ListaUsuarios, 2, "", str2, ESPERANDO_PASS);
			usr = UsuariosADS_BuscarUsr(&ADS.m_ListaUsuarios, 2, &i);
			UsuariosADS_AgregarUsr(&ADS.m_ListaUsuarios, 1, "", str2, ESPERANDO_PASS);
			ADS_GetClaveByConnId(2, "/home/miguel/emulaso/claves");
			AplicarXorEnString(str2, ADS_GetClaveByConnId(2, "/home/miguel/emulaso/claves"));
			AplicarXorEnString(str2, ADS_GetClaveByConnId(2, "/home/miguel/emulaso/claves"));
			printf(str2);
		}
	
		return OK;
		
	}while(0);
	
	if ( errno )
	{
		Log_log( log_error, conn_getLastError() );
	}
	else
	{
		printf("No se puede crear el ADS: revise el archivo de configuracion\n");
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
		if ( (tmp = config_GetVal( cfg, _ADS_, "ADS_IP" ) ) )
		{
			strncpy( ADS.m_IP, tmp, LEN_IP );
		}
		
		ADS.m_Port = config_GetVal_Int( cfg, _ADS_, "ADS_PORT" );
		
		strcpy(ADS.m_PathUsuarios, config_GetVal( cfg, _ADS_, "PATH_USU"));
		
		strcpy(ADS.m_PathClavesUsuarios, config_GetVal( cfg, _ADS_, "PATH_KEY_USU"));

		config_Destroy(cfg);

		return OK;
		
	}while(0);
	
	return ERROR;
}

/**********************************************************************/
int ADS_ConectarACR()
{
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	if ( !( pSocket = conexiones_ConectarHost( ADS.m_ACR_IP, ADS.m_ACR_Port,
										 &ADS_ConfirmarConexion ) ) )
	{
		return ERROR;
	}
	
	ADS.m_ListaSockets[ SOCK_ACR ] = pSocket;
	ADS.m_ultimoSocket = SOCK_ACR;
	
	/*Mando el Ping al ACR*/
	Log_log( log_debug, "envio Ping para conectarme con ACR" );
	
	if ( !(pPaq  = paquetes_newPaqPing( szIP, _ID_MSHELL_, conexiones_getPuertoLocalDeSocket(pSocket) )) )
		return ERROR;
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	
	paquetes_destruir( pPaq );
	
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
}
/**********************************************************/
void ADS_ConfirmarConexion( tSocket* sockIn )
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
		conexiones_CerrarSocket( ADS.m_ListaSockets, sockIn, &ADS.m_ultimoSocket );
		return;
	}
	
		
	paq = paquetes_CharToPaq( buffer );

	if ( IS_ACR_PAQ( paq ) &&  IS_PAQ_PONG ( paq ) )
	{/*Si el ADS me responde pong la conexion queda establecida!*/
				
		Log_log( log_debug, "Conexion establecida con el ACR!" );
		sockIn->callback = &ADS_AtenderACR;
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
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
		
		/*Mando el Pong al Mshell*/
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

	if ( IS_PAQ_LOGIN_USR( paq ) )
	{/*Me llega el nombre de usuario*/
		tPaquete *paqSend = NULL;
		char szUserName[LEN_USERNAME] = {'\0'};
		unsigned char szIP[4];
		int nSend;
			
		memset( szIP, 0, 4 );
		
		paquetes_ParsearUserName(buffer, szUserName);
		/*verificarExistenciaUsuario en archivo de usuarios*/
		if ( ADS_BuscarUsuario(ADS.m_PathUsuarios, szUserName ) )
		{
			UsuariosADS_AgregarUsr(&(ADS.m_ListaUsuarios), sockIn->descriptor, paq->id.IP, szUserName, ESPERANDO_PASS);
			
			if ( !(paqSend  = paquetes_newPaqUserNameOk( szIP, _ID_ADS_, conexiones_getPuertoLocalDeSocket(sockIn) )) )
			{
				Log_log( log_error, "Error enviando UserNameOk al MShell" );
			}
			Log_log( log_debug, "Informo que el UserName es valido" );
			
		}
		else
		{
			if ( !(paqSend  = paquetes_newPaqUserNameInvalido( szIP, _ID_ADS_, conexiones_getPuertoLocalDeSocket(sockIn) )) )
			{
				Log_log( log_error, "Error enviando UserNameInvalido al MShell" );
			}
			Log_log( log_debug, "Informo que el UserName no es valido" );
		}
		nSend = conexiones_sendBuff( sockIn,(const char*)paquetes_PaqToChar( paqSend ), PAQUETE_MAX_TAM );
		if ( nSend != PAQUETE_MAX_TAM )
		{
			Log_logLastError( "error enviando Validacion UserName " );
		}
		paquetes_destruir( paqSend );
	}
	else /*es un paquete encriptado*/
	{	
		paquetes_destruir(paq);
		paq = paquetes_CharToPaq((const char*)AplicarXorEnString(buffer, ADS_GetClaveByConnId(sockIn->descriptor, ADS.m_PathClavesUsuarios)));
		
		
		if ( IS_PAQ_LOGIN_PWD( paq ) )
		{/*Me llega el password */
			
			/*precondicion: el Mshell sabe que no puede mandar otra ves el pass, si ya esta logueado*/
			tUsuarioADS* usrBuscado = NULL;
			int iPos = 0;
			char szPassword[LEN_PASSWORD];
			tPaquete *paqSend = NULL;
			unsigned char szIP[4];
			int nSend;
			
			memset( szIP, 0, 4 );
			
			if((usrBuscado = UsuariosADS_BuscarUsr(&(ADS.m_ListaUsuarios),sockIn->descriptor, &iPos))==NULL)
			{
				return;
			}
			paquetes_ParsearPassword(buffer, szPassword);
			if(ADS_ValidarPassword(usrBuscado->Usuario, szPassword, ADS.m_PathUsuarios))
			{
				usrBuscado->Estado = CONECTADO;
				if ( !(paqSend  = paquetes_newPaqPasswordOk( szIP, _ID_ADS_, conexiones_getPuertoLocalDeSocket(sockIn) )) )
				{
					Log_log( log_error, "Error enviando Welcome al MShell" );
				}
				Log_log( log_debug, "Informo que el usuario esta logueado" );
				nSend = conexiones_sendBuff( sockIn,  (const char*)AplicarXorEnString((char*)paquetes_PaqToChar( paqSend ), ADS_GetClaveByConnId(sockIn->descriptor, ADS.m_PathClavesUsuarios)), PAQUETE_MAX_TAM );
				if ( nSend != PAQUETE_MAX_TAM )
				{
					Log_logLastError( "error enviando Welcome al MShell" );
				}
			}
			else
			{
				if ( !(paqSend  = paquetes_newPaqPasswordInvalido( szIP, _ID_ADS_, conexiones_getPuertoLocalDeSocket(sockIn) )) )
				{
					Log_log( log_error, "Error enviando pasword invalido al MShell" );
				}
				Log_log( log_debug, "Informo que el usuario no se pudo loguear" );
				nSend = conexiones_sendBuff( sockIn, (const char*)AplicarXorEnString((char*)paquetes_PaqToChar( paqSend ), ADS_GetClaveByConnId(sockIn->descriptor, ADS.m_PathClavesUsuarios)), PAQUETE_MAX_TAM );
				if ( nSend != PAQUETE_MAX_TAM )
				{
					Log_logLastError( "error enviando pasword invalido al MShell" );
				}
				UsuariosADS_EliminarUsr(&(ADS.m_ListaUsuarios), sockIn->descriptor);
				ADS_CerrarConexion(sockIn);
			}
			paquetes_destruir( paqSend );
		}
		else if(IS_PAQ_LOGOUT(paq))
		{
			tPaquete *paqSend = NULL;
			unsigned char szIP[4];
			int nSend;
			
			memset( szIP, 0, 4 );

			ADS_CerrarConexion(sockIn);
			
			if ( !(paqSend  = paquetes_newPaqADSLogout( szIP, _ID_ADS_, conexiones_getPuertoLocalDeSocket(sockIn),sockIn->descriptor )) )
			{
				Log_log( log_error, "Error enviando Logout al ACR" );
			}
			Log_log( log_debug, "Informo al ACR que se desconecto un Usuario" );
			nSend = conexiones_sendBuff( ADS.m_ListaSockets[SOCK_ACR], (const char*)paquetes_PaqToChar( paqSend ), PAQUETE_MAX_TAM );
			if ( nSend != PAQUETE_MAX_TAM )
			{
				Log_logLastError( "Error enviando Logout de un usuario al ACR" );
			}
			
			UsuariosADS_EliminarUsr(&(ADS.m_ListaUsuarios), sockIn->descriptor);
			
			paquetes_destruir( paqSend );
		}
		else if(IS_PAQ_EXEC_PROG(paq))/*TODO: verificar q info necesita el ACR*/
		{
			int nSend;
			
			nSend = conexiones_sendBuff( ADS.m_ListaSockets[SOCK_ACR], (const char*)paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
			if ( nSend != PAQUETE_MAX_TAM )
			{
				Log_logLastError( "Error enviando Exec al ACR" );
			}
		}
		
		if ( paq )
		{
			paquetes_destruir( paq );
		}
	}
	
}


/**********************************************************/
char* ADS_BuscarUsuario(const char* szPathUsuarios, const char* userName)
{
	char szLinea[LEN_MAX_LINEA_ARCH_USUARIOS] = {'\0'};
	FILE *fp;
	
	if( ( fp = fopen( szPathUsuarios, "r" ) ) == NULL )
	{
		printf("Error al abrir el archivo de Usuarios\n");
		return NULL;
	}
	while(fgets(szLinea,LEN_MAX_LINEA_ARCH_USUARIOS, fp))
	{
		char szUserName[LEN_USERNAME] = {'\0'};
		
		ADS_ExtraerUserName(szUserName, szLinea);
		if (strcmp(userName, szUserName) == 0)
		{
			fclose(fp);
			return (char*)userName;
		}
	}
	fclose(fp);
	return NULL;
}

/**********************************************************/
void ADS_ExtraerUserName(char* szUserNAme, const char* szLinea)
{
	char szLineaCpy[LEN_MAX_LINEA_ARCH_USUARIOS] = {'\0'};
	
	strcpy(szLineaCpy, szLinea);
	strcpy(szUserNAme, strtok(szLineaCpy, ":"));
	return;
}

/**********************************************************/
char *  ADS_ValidarPassword(const char *szUsername, const char *szPassword, const char *szPathUsuarios)
{
	char szLinea[LEN_MAX_LINEA_ARCH_USUARIOS] = {'\0'};
		FILE *fp;
		
		if( ( fp = fopen( szPathUsuarios, "r" ) ) == NULL )
		{
			printf("Error al abrir el archivo de Usuarios\n");
			return NULL;
		}
		while(fgets(szLinea,LEN_MAX_LINEA_ARCH_USUARIOS, fp))
		{
			char szUserNameExt[LEN_USERNAME] = {'\0'};
			char szPasswordExt[LEN_PASSWORD] = {'\0'};
			
			ADS_ExtraerUserName(szUserNameExt, szLinea);
			ADS_ExtraerPassword(szPasswordExt, szLinea);
			if ((strcmp(szUsername, szUserNameExt) == 0) && (strcmp(szPassword, szPasswordExt) == 0))
			{
				fclose(fp);
				return (char*)szPassword;
			}
		}
		fclose(fp);
		return NULL;
}

/**********************************************************/
void 	ADS_ExtraerPassword(char* szPassword, const char* szLinea)
{
	char szLineaCpy[LEN_MAX_LINEA_ARCH_USUARIOS] = {'\0'};
	strcpy(szLineaCpy,szLinea);
	strtok(szLineaCpy, ":");
	strcpy(szPassword, strtok(szLineaCpy, NULL));
	return;
}
/**********************************************************/
int ADS_GetClaveByConnId(int connId, const char *pathClaves )
{
	char szNombreArchivoClave[LEN_PATH_USUARIOS] = {'\0'};
	int iPos = 0;
	tUsuarioADS *usr = NULL;
	FILE *fp;
	int key;
	
	if((usr = UsuariosADS_BuscarUsr(&(ADS.m_ListaUsuarios),connId, &iPos))==NULL)
	{
		printf("Error al abrir el archivo de Clave\n");
		return -1;
	}
	if(usr->key != -1)
	{
		return usr->key;
	}
	strcpy(szNombreArchivoClave, pathClaves);
	strcat(szNombreArchivoClave, "/");
	strcat(szNombreArchivoClave, usr->Usuario);
	strcat(szNombreArchivoClave, TERMINACION_ARCHIVO_CLAVE);
	
	if( ( fp = fopen( szNombreArchivoClave, "r" ) ) == NULL )
	{
		printf("Error al abrir el archivo de Clave\n");
		return -1;
	}
	key = fgetc(fp);
	
	fclose(fp);
	
	usr->key = key;
 	
	return usr->key;
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
	conexiones_CerrarSocket( ADS.m_ListaSockets, sockIn, &ADS.m_ultimoSocket );
}
 
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
