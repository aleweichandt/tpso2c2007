/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: MShellLib.h
 * 
 * Historia
 *	03-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */
 
 #include "MShellLib.h"
 
 /*Variables privadas*/
sigset_t 		conjunto_seniales;
char			isUsrLogued;	//1 si el uusario esta logueado, 0 si no lo esta
char			isExit;		//se�al para separar cuando cierra sesion de cuando cierra el mshell
char			usrName[15];
char			*usrPath;

/**************************************************\
 *            PROTOTIPOS DE FUNCIONES Privadas     *
\**************************************************/

						




/**********************************************************/
void MSH_ProcesarSeniales( int senial )
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
		MSH_Salir();
	}
	else if ( senial == SIGCHLD )
	{	
		Log_log( log_warning, "Recibo senial SIGCHILD");
		wait( &nstateChld );
		Log_log( log_warning, "Certifico que murio un proc hijo(ADT)");
		signal( SIGCHLD, &MSH_ProcesarSeniales );
	}
}

/***********************************************************************************/
void MSH_CrearGraficos( int activarGraficos )
{
	#define X_REMOTO		1
	#define Y_REMOTO		1
	#define ANCHO_REMOTO	49
	#define ALTO_REMOTO		17

	#define X_CONTROL		ANCHO_REMOTO+1
	#define Y_CONTROL		Y_REMOTO
	#define ANCHO_CONTROL	PANTALLA_COLS-ANCHO_REMOTO
	#define ALTO_CONTROL	ALTO_REMOTO

	#define X_LOCAL			X_REMOTO
	#define Y_LOCAL			ALTO_REMOTO+1
	#define ANCHO_LOCAL		PANTALLA_COLS
	#define ALTO_LOCAL		PANTALLA_ROWS-ALTO_REMOTO	
	
	if ( activarGraficos )
	{
		pantalla_Clear();
		
		MShell.m_pwRemoto  = ventana_Crear(X_REMOTO, 	Y_REMOTO, 	ANCHO_REMOTO, 	ALTO_REMOTO, 	1, "Remoto");
		MShell.m_pwLocal 	= ventana_Crear(X_LOCAL, 	Y_LOCAL, 	ANCHO_LOCAL, 	ALTO_LOCAL, 	0, "MSHell");
	}
	else
	{
		MShell.m_pwRemoto = MShell.m_pwLocal = NULL;
	}
}

/**********************************************************************/
int MSH_Init( )
{
	loginSig = 0;
	isExit = 0;
	isUsrLogued = 0;
	do
	{
		Log_Inicializar( _MSHELL_, "1" );
		
		Log_log( log_debug, "Inicio de Aplicacion" );
		
				
		if ( MSH_LeerConfig() == ERROR )
		{
			Log_log( log_error, "Error leyendo la configuracion" ); 
			break;
		}
				
		/* inicializacion de la lista de sockets */
		if ( !(MShell.m_ListaSockets = malloc( MALLOC_SOCKS_INI *sizeof(tSocket*) ) ) ) 
			return ERROR;
			
		memset( MShell.m_ListaSockets, 0, MALLOC_SOCKS_INI * sizeof(tSocket*) );
		
		/*Creo el socket para el teclado*/
		MShell.m_ListaSockets[ SOCK_TECLADO ] = conexiones_CrearSocketAsociado( STDIN_FILENO, 
														&MSH_ProcesarTeclado );
		MShell.m_ultimoSocket = SOCK_TECLADO;
		
		MSH_CrearGraficos( 1 );
		
		signal(SIGALRM, MSH_ProcesarSeniales);
		signal(SIGCHLD, MSH_ProcesarSeniales);
	
		return OK;
		
	}while(0);
	
	if ( errno )
	{
		ventana_Printf( MShell.m_pwRemoto, conn_getLastError() );
	}
	else
	{
		printf("No se puede crear el Buscador: revise el archivo de configuracion\n");
	}
		
	exit(EXIT_FAILURE);
	
	return ERROR;
}

/**********************************************************************/
int MSH_LeerConfig()
{
	char 	*tmp;
	tConfig *cfg;
	
	do
	{		
		if ( !(cfg = config_Crear( "config", _MSHELL_ )) ) 
			break;
		
		/*Levanto la configuracion*/
		if ( (tmp = config_GetVal( cfg, _MSHELL_, "ADS_IP" ) ) )
		{
			strncpy( MShell.m_ADS_IP, tmp, LEN_IP );
		}
		
		MShell.m_ADS_Port = config_GetVal_Int( cfg, _MSHELL_, "ADS_PORT" );
		if ( (tmp = config_GetVal( cfg, _MSHELL_, "PATH_USU" ) ) )
		{
			strcpy( usrPath, tmp );
		}
		
		config_Destroy(cfg);

		return OK;
		
	}while(0);
	
	return ERROR;
}

/**********************************************************************/
int MSH_ConectarADS()
{
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	if ( !( pSocket = conexiones_ConectarHost( MShell.m_ADS_IP, MShell.m_ADS_Port,
										 &MSH_ConfirmarConexion ) ) )
		return ERROR;
	
	MShell.m_ListaSockets[ SOCK_ADS ] = pSocket;
	MShell.m_ultimoSocket = SOCK_ADS;
	
	/*Mando el Ping al ADS*/
	Log_log( log_debug, "envio Ping para conectarme con ADS" );
	
	if ( !(pPaq  = paquetes_newPaqPing( MShell.m_ADS_IP, _ID_MSHELL_, conexiones_getPuertoLocalDeSocket(pSocket) )) )
		return ERROR;
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	
	paquetes_destruir( pPaq );
	
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
}

/**********************************************************/
void MSH_ConfirmarConexion( tSocket* sockIn )
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
		conexiones_CerrarSocket( MShell.m_ListaSockets, sockIn, &MShell.m_ultimoSocket );
		return;
	}
	
		
	paq = paquetes_CharToPaq( buffer );

	if ( IS_ADS_PAQ( paq ) &&  IS_PAQ_PONG ( paq ) )
	{/*Si el ADS me responde pong la conexion queda establecida!*/
		ventana_Print( MShell.m_pwRemoto, "Bienvenido a B&RR MShell. Debe logearse.");
		
		Log_log( log_debug, "Conexion establecida con el ADS!" );
		sockIn->callback = &MSH_AtenderADS;
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
}

/**********************************************************/
void MSH_AtenderADS( tSocket* sockIn )
{
	char* 			tmp; 
	int 			len; 
	char 			buffer[ PAQUETE_MAX_TAM ];
	tPaquete* 		paq = NULL;
	
	len = conexiones_recvBuff( sockIn, buffer, 66 );
	
	if ( ERROR == len || !len)
	{
		conexiones_CerrarSocket( MShell.m_ListaSockets, sockIn, &MShell.m_ultimoSocket );
		return;
	}
	
		
	paq = paquetes_CharToPaq( buffer );

	if ( IS_PAQ_PONG ( paq ) )
	{/*Si el ADS me responde pong la conexion queda establecida!*/
		Log_log( log_debug, "Conexion establecida con el ADS, se envia usuario!" );
		if(!MSH_Login_Send(usrName, 0))
		{
			Log_log( log_error, "MShell No Envio Usuario a ADS!!" );
			break;
		}
	}
	if ( IS_PAQ_USR_OK ( paq ) )
	{/*Si el ADS me responde Usr_Ok se pide contrase�a!*/
		MShell.m_ListaSockets[ SOCK_TECLADO ]->callback = &MSH_ProcesarTecladoIfUsrOk;
		sockIn->callback = &MSH_AtenderADSEncript
		ventana_Print( MShell.m_pwRemoto, "Ingrese Contrase�a:" );
		Log_log( log_debug, "ADS confirma usuario!" );
	}
	if ( IS_PAQ_USR_ERROR ( paq ) )
	{/*Si el ADS me responde Usr_Error no inicia sesion!*/
		
		Log_log( log_debug, "ADS rechaza usuario!" );
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
}

/**********************************************************/
void MSH_AtenderADSEncript ( tSocket *sockIn )
/**/
{
	int 			len; 
	tPaquete* 		paq ; 
	char 			*tmp;
	char			buffer [ PAQUETE_MAX_TAM ]; 
	tIDMensaje		idMsj;


	len = conexiones_recvBuff(sockIn, buffer, 66);
	
	if ( ERROR == len || !len)
	{
		conexiones_CerrarSocket( MShell.m_ListaSockets, sockIn, 
			&MShell.m_ultimoSocket );
			
		ventana_Print( MShell.m_pwRemoto, "Atencion! Se cayo el ADS!!" );
		Log_log( log_error, "Atencion! Se cayo el ADS!!" );
		
	
		return;
	}
	
	Log_log( log_debug, "Me llega un paquete del ADS" );
	
	paq = paquetes_CharToPaq(MSH_Encript(buffer));

	if ( IS_PAQ_PWD_OK ( paq ) )
	{/*Si el ADS me responde Pwd_ok la sesion queda iniciada!*/
		isUsrLogued = 1;
		Log_log( log_debug, "ADS acepta password, sesion iniciada!" );
	}
	if ( IS_PAQ_PWD_ERROR ( paq ) )
	{/*Si el ADS me responde Pwd_Error no inicia sesion!*/
		
		Log_log( log_debug, "ADS rechaza password!" );
	}
	if ( IS_PAQ_PROG_EXECUTING ( paq ) )
	{/*Si el ADS ejecuta el programa!*/
		memset(tmp,0,50);
		strcpy(tmp,paq->msg);
		strcat(tmp," se esta ejecutando");
		ventana_Print( MShell.m_pwRemoto, tmp );
		Log_log( log_debug, "ADS acepta exec!" );
	}
	if ( IS_PAQ_NO_PROG ( paq ) )
	{/*Si el ADS no ejecuta el programa!*/
		memset(tmp,0,50);
		strcpy(tmp,paq->msg);
		strcat(tmp," o se ejecuto, o no es un programa");
		ventana_Print( MShell.m_pwRemoto, tmp );
		Log_log( log_debug, "ADS rechaza exec!" );
	}
	if ( IS_PAQ_END_SESION_OK ( paq ) )
	{/*Si el ADS me responde ok al logout !*/
		isUsrLogued = 0;
		Log_log( log_debug, "ADS confirma logout!" );
		/*se cierra la conexion con ADS*/
		conexiones_CerrarSocket( MShell.m_ListaSockets, sockIn, &MShell.m_ultimoSocket );
		if(isExit)
		{
			Log_log( log_debug, " se va a Finalizar MSHELL!" );
			MSH_Salir();
		}
	}
	if ( IS_PAQ_PRINT ( paq ) )
	{/*Si el ADS envia un print!*/
		Log_log( log_debug, "ADS envio un print!" );
		ventana_Print( MShell.m_pwRemoto, "Mensaje recibido:\n" );
		ventana_Print( MShell.m_pwRemoto, paq->msg );
	}	
				
	if ( paq ) 
		paquetes_destruir( paq );
	
}

/**********************************************************/
void MSH_ProcesarTeclado(tSocket* sockIn)
/**/
{
	int i, largo;
	
	/* sockIn siempre es stdin, pero es necesario tenerlo para usar el callback */
	#define LEN_COMANDO 4
	char cmd[_LARGO_CMD_]; 
	char *p;
	int cant;
	
	memset(cmd,0,sizeof(cmd));

	cant = getCadT(stdin, '\n', cmd);
	
	do
	{
		
		if (!cant || !strlen(cmd)) 
			break;
		
		p=strtok(cmd," ")
		if(!isUsrLogued)
		{
			if( strcmp ( p, "login" ) == 0 ){
				p=strtok(NULL," ");
				strcpy(usrName, p);
				if ( MSH_ConectarADS() == ERROR )
				{
					Log_log( log_error, "Error conectandose con el ADS!" );
					break;
				}
			}
		}
		else
		{
			if( strcmp ( p, "exec" ) == 0 )
			{
				p=strtok(NULL," ");
				if(!MSH_exec(p))
				{
					Log_log( log_error, "MShell No Envio exec a ADS!!" );
					break;
				}
			}
			if( strcmp ( p, "logout" ) == 0 )
			{
				if(!MSH_Logout(p))
				{
					Log_log( log_error, "MShell No Envio exec a ADS!!" );
					break;
				}
			}
			if( strcmp ( p, "exit" ) == 0 )
			{
				if(!MSH_Logout(p))
				{
					Log_log( log_error, "MShell No Envio exec a ADS!!" );
					break;
				}
				isExit = 1;
			}
		}
		
		ventana_Clear( MShell.m_pwLocal );
		
	}while(0);	
		
}

/**********************************************************/
void MSH_ProcesarTecladoIfUsrOk(tSocket* sockIn)
/**/
{
	int i, largo;
	
	/* sockIn siempre es stdin, pero es necesario tenerlo para usar el callback */
	#define LEN_COMANDO 4
	char cmd[_LARGO_CMD_]; 
	char *p;
	int cant;
	
	memset(cmd,0,sizeof(cmd));

	cant = getCadT(stdin, '\n', cmd);
	
	do
	{
		
		if (!cant || !strlen(cmd)) 
			break;
		
		if(!MSH_Login_Send(cmd,1))
		{
			Log_log( log_error, "MShell No Envio Password a ADS!!" );
			break;
		}
		MSH_Set_UsrFile(cmd);
		sockIn->callback = &MSH_ProcesarTeclado;
		ventana_Clear( MShell.m_pwLocal );
		
		
	}while(0);	
		
}

/**********************************************************/
void MSH_Salir()
{
	/* Logear salida, hacer un clean up? */
	ventana_Destruir( MShell.m_pwLocal );
	ventana_Destruir( MShell.m_pwRemoto );
	
	conexiones_CerrarLista( 0, &MShell.m_ultimoSocket, MShell.m_ListaSockets );
	
	Log_log(log_info,"Fin de la ejecucion");
	pantalla_Clear();
	exit(EXIT_SUCCESS);
}


/**********************************************************/
int MSH_Login_Send(char msj[15], char isPwd)
{
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	if(!isPwd)
	{
		/*Mando el Usr al ADS*/
		Log_log( log_debug, "envio Usuario al ADS" );
	
		if ( !(pPaq  = paquetes_newPaqLogin_Usr( MShell.m_ADS_IP, _ID_MSHELL_, conexiones_getPuertoLocalDeSocket(pSocket),msj )) )
			return ERROR;
	
		nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	
		paquetes_destruir( pPaq );
	
		return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
		
	} else {
		
		/*Mando el Pwd al ADS*/
			Log_log( log_debug, "envio Password al ADS" );
			
			if ( !(pPaq  = paquetes_newPaqLogin_Usr( MShell.m_ADS_IP, _ID_MSHELL_, conexiones_getPuertoLocalDeSocket(pSocket),msj )) )
				return ERROR;
			
			nSend = conexiones_sendBuff( pSocket, (const char*) MSH_Encript(paquetes_PaqToChar( pPaq )), PAQUETE_MAX_TAM );
			
			paquetes_destruir( pPaq );
			
			return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
	}
}

/**********************************************************/
int MSH_Exec_Prog(char prog[30])
{
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	/*Mando el programa al ADS*/
	Log_log( log_debug, "envio nombre del programa al ADS" );
	
	if ( !(pPaq  = paquetes_newPaqExec_Prog( MShell.m_ADS_IP, _ID_MSHELL_, conexiones_getPuertoLocalDeSocket(pSocket),prog )) )
		return ERROR;
	
	nSend = conexiones_sendBuff( pSocket, (const char*) MSH_Encript(paquetes_PaqToChar( pPaq )), PAQUETE_MAX_TAM );
	
	paquetes_destruir( pPaq );
	
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
		
}

/**********************************************************************/
int MSH_Logout()
{
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	/*Mando el Ping al ADS*/
	Log_log( log_debug, "envio Aviso de Logout al ADS" );
	
	if ( !(pPaq  = paquetes_newPaqLogout( MShell.m_ADS_IP, _ID_MSHELL_, conexiones_getPuertoLocalDeSocket(pSocket) )) )
		return ERROR;
	
	nSend = conexiones_sendBuff( pSocket, (const char*) MSH_Encript(paquetes_PaqToChar( pPaq )), PAQUETE_MAX_TAM );
	
	paquetes_destruir( pPaq );
	
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
}

/***************************************************************************/
int MSH_Set_UsrFile(char pwd[15])
{
	File *f;
	
	if((f = fopen(usrPath + usrName + ".key", "w")==NULL){
		Log_log( log_error, "no se pudo crear el archivo" + usrName + ".key" );
		return 0;
	}
	fprintf(f,pwd);
	fclose(f);
	
	return 1;
}
/**********************************************************/
int MSH_Encript(char *cad)
{
	File *f
	int i;
	char k;
	
	if((f = fopen(usrPath + usrName + ".key", "r")==NULL){
		Log_log( log_error, "no se pudo abrir el archivo" + usrName + ".key" );
		return 0;
	}
	k=fgetc(f);
	fclose(f);
		
	for ( i = 0; i < strlen(cad); i++ )
		cad[i]=cad[i] XOR k;
	return 1;
}


/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/