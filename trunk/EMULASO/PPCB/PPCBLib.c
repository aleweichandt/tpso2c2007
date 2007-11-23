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

/**************************************************\
 *            PROTOTIPOS DE FUNCIONES Privadas     *
\**************************************************/
int PCB_Migrar(char szIp[LEN_IP], unsigned short int nPuerto);

int PCB_ExecuteInstruction(int line) ;
int PCB_ExecuteMem(char *param);
int PCB_ExecuteOper(char *param);
int PCB_ExecuteSol(char *param);
int PCB_ExecuteDev(char *param);
int PCB_ExecuteImp(char *param);
int PCB_ExecutePush(char *param);
int PCB_ExecutePop(char *param);

int PCB_RemainingTimeExecution();
void SetRemainingTime(int remainingTimeExecution);

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
	{	/* Escribir informacion de control */
		if( PCB.nIdProcesoPadre == _ID_ADP_ )
		{
			createPCBConfig();
		}
		PCB_ImprimirInfoCtr();
		signal(SIGUSR1, PCB_ProcesarSeniales);
		
	}
	else if ( senial == SIGUSR2 )
	{	/*Migrar al ACR*/
		if( PCB.nIdProcesoPadre == _ID_ADP_ )
		{
			Log_log( log_warning, "Recibo senial SIGUR2");
			if( PCB.State == EJECUTANDO ) PCB.State = LISTO;  /*Evito que siga ejecutando en el ACR*/
			PCB_Migrar(PCB.m_ACR_IP,PCB.m_ACR_Port);
		}
		signal(SIGUSR2, PCB_ProcesarSeniales);
	}
	else if ( senial == SIGHUP )
	{
	}
	else if ( senial == SIGINT )
	{			
		Log_log( log_warning, "Recibo senial SIGINT");
		/*PCB_Salir();*/
	}
	else if ( senial == SIGTERM )
	{	
		Log_log( log_warning, "Recibo senial SIGTERM");		
		PCB_Salir();
	}
	else if ( senial == SIGCHLD )
	{	
		Log_log( log_warning, "Recibo senial SIGCHILD");
		signal(SIGCHLD, PCB_ProcesarSeniales);
	}
}


/**********************************************************************/
int createPCBConfig() {

	FILE *cfgFile;
	tConfig *cfg;
	int line=0;
	char strBuff[50];
	char strVal[10];
		
	do {
		/*
		%PPCB%
		<ID>=1
		<CREATORID>=Gonzalo
		<CODE1>=MEM 10
		<CODE2>SOL Impresora
		<CODE3>OPER 15
		<CODE4>DEV Impresora
		<CODE5>IMP Ya Imprimi!!!
		…
		
		<IPOINTER>=2
		<ESTADO>=PENDIENTE
		<STACK1>=A
		<STACK2>=b
		<STACK3>=15
		…
		<IP_ACR>=127.0.0.1
		<PUERTO_ACR>=9000
		
		*/
		
		sprintf(strBuff, "config.ppcb%ld", PCB.PPCB_ID);
		
		if( ExistPath(strBuff) != ERROR && PCB.argc == 3 )
		{	/*existe el archivo*/
		
			if ( !(cfg = config_Crear( strBuff, _PPCB_ )) ) 
				break;	
			
			bzero(strVal,sizeof(strVal));

			sprintf(strVal,"%d",PCB.IP);
			config_SetVal(cfg,_PPCB_,"IPOINTER",strVal);
			
			sprintf(strVal,"%d",PCB.tiempoRestanteOper);
			config_SetVal(cfg,_PPCB_,"TMP_REST_OPER",strVal);
			
			if( PCB.State == PENDIENTE ) 
				config_SetVal(cfg,_PPCB_,"ESTADO","PENDIENTE");
			else if( PCB.State == BLOQUEADO )
				config_SetVal(cfg,_PPCB_,"ESTADO","BLOQUEADO");
			else if( PCB.State == EJECUTANDO )
				config_SetVal(cfg,_PPCB_,"ESTADO","EJECUTANDO");
			else if( PCB.State == LISTO )
				config_SetVal(cfg,_PPCB_,"ESTADO","LISTO");
			
			sprintf(strVal,"%d",PCB_RemainingTimeExecution());
			config_SetVal(cfg,_PPCB_,"TMP_REST",strVal);
			
			config_Guardar(cfg,strBuff);
			
			config_Destroy(cfg);
			
		}else
		{	/*No existe el archivo*/
			cfgFile = fopen(strBuff, "w+");
			
			fprintf(cfgFile, "%PPCB%\n");
			fprintf(cfgFile, "<ID>=%ld\n", PCB.PPCB_ID);
			fprintf(cfgFile, "<CREATORID>=%s\n", PCB.User);
			fprintf(cfgFile, "<COMANDO>=%s\n",  PCB.ProgName);
			fprintf(cfgFile, "<SESIONID>=%d\n", PCB.SessionID);
			
			Log_printf(log_debug,"Config(createConfig): PCB.ultimaSentencia %d",PCB.ultimaSentencia);
			while( line < PCB.ultimaSentencia ) {
				fprintf(cfgFile, "<CODE%d>=%s", line+1, PCB.Code[line]);
				line++;
			}
			
			fprintf(cfgFile, "\n<IPOINTER>=%d\n", PCB.IP);
			
			if( PCB.State == PENDIENTE ) 
				fprintf(cfgFile, "<ESTADO>=%s\n", "PENDIENTE");
			else if( PCB.State == BLOQUEADO )
				fprintf(cfgFile, "<ESTADO>=%s\n", "BLOQUEADO");
			else if( PCB.State == EJECUTANDO )
				fprintf(cfgFile, "<ESTADO>=%s\n", "EJECUTANDO");		
			else if( PCB.State == LISTO )
				fprintf(cfgFile, "<ESTADO>=%s\n", "LISTO");		
			
			fprintf(cfgFile,"<IP_ACR>=%s\n",  PCB.m_ACR_IP);
			
			fprintf(cfgFile,"<PUERTO_ACR>=%d\n", PCB.m_ACR_Port);
			
			fprintf(cfgFile,"<TMP_REST_OPER>=%d\n", PCB.tiempoRestanteOper);
			
			fprintf(cfgFile, "<TMP_REST>=%d\n", PCB_RemainingTimeExecution());
			
			fclose(cfgFile);
		}
		
		return OK;
		
	} while (0);
	
	return ERROR;
}

/**********************************************************************/
int createPCB(char *argv[]) {

	FILE *cfgCode;
	int line=0;
	char strBuff[50];
		
	do {
		/*
		%PPCB%
		<ID>=1
		<CREATORID>=Gonzalo
		<CODE1>=MEM 10
		<CODE2>SOL Impresora
		<CODE3>OPER 15
		<CODE4>DEV Impresora
		<CODE5>IMP Ya Imprimi!!!
		…
		
		<IPOINTER>=2
		<ESTADO>=PENDIENTE
		<STACK1>=A
		<STACK2>=b
		<STACK3>=15
		…
		<IP_ACR>=127.0.0.1
		<PUERTO_ACR>=9000
		
		*/
		
		strcpy( PCB.ProgramPath, argv[7] );
		
		sprintf(strBuff, "%s/%s", PCB.ProgramPath, argv[3] );
		cfgCode = fopen(strBuff, "r+");
		
		strcpy( PCB.User, argv[2] );
		
		strcpy( PCB.ProgName, argv[3] );
		
		PCB.SessionID = atoi (argv[4]);
		
		bzero(strBuff,sizeof(strBuff));
		while (!feof(cfgCode) && fgets(strBuff, 200, cfgCode)) {
			strncpy(PCB.Code[line], strBuff,50 );
			line++;
		}
		PCB.ultimaSentencia = line;
		Log_printf(log_debug,"Config(createPPCB): PCB.ultimaSentencia %d",PCB.ultimaSentencia);
		sscanf( PCB.Code[0], "MEM %d", &(PCB.Mem));	/*tomo la cantidad de memoria requerida*/
		
		PCB.IP = 0;
		
		PCB.State = PENDIENTE;
		
		strncpy( PCB.m_ACR_IP, argv[5], LEN_IP );
		
		PCB.m_ACR_Port = atoi(argv[6]);
		
		fclose(cfgCode);
		/* PARA PRUEBA */
		/*PCB_ExecuteProgram();*/
		return OK;
		
	} while (0);
	
	return ERROR;
}
/***********************************************************/
void PCB_ExecuteProgram(tSocket *sockIn) {
	char szaux[50];
	
	/*Log_log(log_debug,"Entre en el Execute program del timeout");*/
	if ( PCB.State == EJECUTANDO && PCB.IP < PCB.ultimaSentencia) {
		
		if( !PCB.tiempoRestanteOper )	/*La siguiente sentencia*/
		{
			PCB_ExecuteInstruction(PCB.IP);
			PCB.IP++;
		}else							/*Una sentencia OPER sin terminar*/
		{
			/*sleep(1);*/
			PCB.tiempoRestanteOper--;
			
			if(!PCB.tiempoRestanteOper)
				Log_log( log_info, "Se termino de ejecutar la operacion");
		}
	}else if(PCB.State == EJECUTANDO && PCB.IP >= PCB.ultimaSentencia){
		
		/*Termino la ejecucion, se envía el mensaje PRINT con*/
		/* "Proceso <PPCBID> finalizado exitosamente" */
		/*PCB_Salir(); /* ¿Esto se hara aca? confirmar con Leonardo */
		/*Hola, soy Leonardo: voy probar matarte yo (ADP)*/
		sprintf( szaux, "Proceso %ld finalizado exitosamente", PCB.PPCB_ID );
		PCB_ExecuteImpFinal( szaux );
		PCB.State = FINALIZADO;
	}
	
}
/**************************************************************/
int PCB_ExecuteInstruction(int line) {
	char sentence[50];
	char instruction[5];
	int (*executer)(char *);
	int paramStart;
	
	strncpy(sentence, PCB.Code[line], 50);
	
	strncpy(instruction, sentence, 4);
	instruction[4] = '\0';
	Log_printf( log_info, "Sentencia: %s", sentence );
	Log_printf( log_info, "Comando: %s", instruction );
	
	if ( !strcmp("MEM ", instruction) ) {
		executer = PCB_ExecuteMem;	
	} else if ( !strcmp("OPER", instruction) ) {
		executer = PCB_ExecuteOper;		
	} else if ( !strcmp("SOL ", instruction) ) {
		executer = PCB_ExecuteSol;	
	} else if ( !strcmp("DEV ", instruction) ) {
		executer = PCB_ExecuteDev;	
	} else if ( !strcmp("IMP ", instruction) ) {
		executer = PCB_ExecuteImp;	
	} else if ( !strcmp("PUSH", instruction) ) {
		executer = PCB_ExecutePush;	
	} else if ( !strncmp("POP", instruction, 3) ) {
		executer = PCB_ExecutePop;	
	}  else {
		Log_log( log_error, "Comando no reconocido, checkee el script");
	}
	
	if ( sentence[3] == ' ' ) {
		paramStart = 4;
	} else if ( sentence[4] == ' ') {
		paramStart = 5;
	}
	
	executer(sentence+paramStart);
	/*SetRemainingTime(PCB_RemainingTimeExecution());*/
	
	return 0;
}

/**********************************************************************/
int PCB_ExecuteMem(char *param) {
	PCB.Mem = atoi(param);
	Log_printf( log_info, "Se solicita Memoria: %s", param);
	
	return 0;
}
/**********************************************************************/
int PCB_ExecuteOper(char *param) {
	int cantSeg = atoi( param);
	
	Log_printf( log_info, "Se ejecuta una operacion de: %s segundos", param);
	
	PCB.tiempoRestanteOper = cantSeg;
	return 0;
}
/**********************************************************************/
int PCB_ExecuteSol(char *param) {
	
	tRecurso recurso;
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	ReducirIP(PCB.m_IP,szIP);
	
	if (PCB_RecursoFromFriendlyName(&recurso, param) == ERROR)
		return;
	
	pPaq = paquetes_newPaqSol(szIP, (unsigned char)_PPCB_, PCB.m_Port, PCB.PPCB_ID, recurso);
	
	Log_printf( log_info, "Se solicita recurso: %s", param);
	
	nSend = conexiones_sendBuff( PCB.m_socketADP, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	if ( nSend != PAQUETE_MAX_TAM )
	{
		Log_logLastError( "error enviando SOL al ADP" );
	}
	PCB.State = BLOQUEADO;
	paquetes_destruir( pPaq );
	
	/*sleep(1);*/
	
	return 0;
}
/**********************************************************************/
int PCB_ExecuteDev(char *param) {
	
	tRecurso recurso;
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	ReducirIP(PCB.m_IP,szIP);
	
	if (PCB_RecursoFromFriendlyName(&recurso, param) == ERROR)
		return;
	
	pPaq = paquetes_newPaqDev(szIP, (unsigned char)_PPCB_, PCB.m_Port, PCB.PPCB_ID, recurso);
	
	Log_printf( log_info, "Ppcb_id:%i devuelve recurso: %s",PCB.PPCB_ID, param);
	
	nSend = conexiones_sendBuff( PCB.m_socketADP, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	if ( nSend != PAQUETE_MAX_TAM )
	{
		Log_logLastError( "error enviando DEV al ADP" );
	}	
	paquetes_destruir( pPaq );
	
	/*sleep(1);*/
	
	return 0;
}
/**********************************************************************/
int PCB_ExecuteImp(char *param) {
	/* MENSAJE PRINT!!!!! */
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	ReducirIP(PCB.m_IP,szIP);
	
	Log_printf( log_info, "Se imprime: %s", param);
	
	pPaq = paquetes_newPaqPrint(szIP, (unsigned char)_PPCB_, PCB.m_Port, PCB.SessionID, PCB.ProgName, param);
	
	Log_log( log_debug, "Mando PRINT al ADP" );
	nSend = conexiones_sendBuff( PCB.m_socketADP, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	if ( nSend != PAQUETE_MAX_TAM )
	{
		Log_logLastError( "error enviando PRINT al ADP" );
	}	
	paquetes_destruir( pPaq );
	
	/*sleep(1);*/
		
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;

}
/**********************************************************************/
int PCB_ExecuteImpFinal(char *param) 
{/*Lo tuve que hacer para no romper el disenio que vienen llevando, esta solo difiere con el 
print comun en que lleva el pcbid*/
	/* MENSAJE PRINT!!!!! */
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	ReducirIP(PCB.m_IP,szIP);
	
	Log_printf( log_info, "Se imprime: %s", param);
	
	pPaq = paquetes_newPaqPrint(szIP, (unsigned char)_PPCB_, PCB.m_Port, PCB.SessionID, PCB.ProgName, param);
	memcpy( &(pPaq->id.UnUsed), &PCB.PPCB_ID, sizeof(PCB.PPCB_ID) );/*Esta es la diferencia*/
	Log_log( log_debug, "Mando PRINT al ADP" );
	nSend = conexiones_sendBuff( PCB.m_socketADP, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	if ( nSend != PAQUETE_MAX_TAM )
	{
		Log_logLastError( "error enviando PRINT al ADP" );
	}	
	paquetes_destruir( pPaq );
	
	/*sleep(1);*/
		
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;

}
/**********************************************************************/
int PCB_ExecutePush(char *param) {
	char value = param[0];
	
	push(&PCB.stack, value);
	Log_printf( log_info, "Se inserta en la pila: %c", value);
	/*MyStackPrint(&PCB.stack);*/
	/*sleep(1);*/
	return 0;
}
/**********************************************************************/
int PCB_ExecutePop(char *param) {
	char value = pop(&PCB.stack);
	Log_printf( log_info, "Se poppea: %c", value);
	
	/*sleep(1);*/
	return 0;
}


/**********************************************************************/
int PCB_Init(int argc, char *argv[] )
{
	do
	{
		Log_Inicializar( _PPCB_, "1" );
		
		PCB.PPCB_ID = atol (argv[1]);
		PCB.tiempoRestanteOper = 0;	/*Se inicializa en cero*/
		PCB.argc = argc;	/*Lo guardo para arreglar error en crear config.ppcb<ID>*/
			
		/*tests*/
		if (0)
		{
			createPCB(argv);
			createPCBConfig();
			PCB_ImprimirInfoCtr();
			return -1;
		}
		if ( argc == 3) {
			
			PCB.nIdProcesoPadre = atoi( argv[2] );	/*el proceso que lo origino (o _ID_ACR_ o _ID_ADP_)*/
						
			if ( PCB_LeerConfig() == ERROR )
			{
				Log_log( log_error, "Error leyendo la configuracion" ); 
				break;
			}
					
		} else if ( argc == 8) {
			
			PCB.nIdProcesoPadre = _ID_ACR_;	/*el unico que usa esta cant de param es el ACR*/
			
			if ( createPCB(argv) == ERROR){
				Log_log( log_error, "Error creando la configuracion" ); 
				break;
			}
			
			
		}
		/* inicializacion de la lista de sockets */
		if ( !(PCB.m_ListaSockets = malloc( MALLOC_SOCKS_INI *sizeof(tSocket*) ) ) ) 
			return ERROR;
			
		memset( PCB.m_ListaSockets, 0, MALLOC_SOCKS_INI * sizeof(tSocket*) );
		
		/*Creo el socket de escucha*/
			
		/*PCB.m_ultimoSocket = SOCK_ESCUCHA; */
		
		if ( argc == 8 ) 
		{
			if ( PCB_ConectarConACR() == ERROR )
			{
				Log_log( log_error, "No se pudo establecer conexion con el ACR" );
				return ERROR;
			}
			/*tSocket* sockDummy = (tSocket*)CrearSocket();
			sockDummy -> estado = estadoConn_standby;
			PCB.m_ListaSockets[SOCK_ADP] = sockDummy;*/
		}
		if ( argc == 3 ) 
		{
			if( PCB.nIdProcesoPadre == _ID_ACR_ )	/*si lo creo el ACR, se conecta con el ACR*/
			{
				if ( PCB_ConectarConACR() == ERROR )
				{
					Log_log( log_error, "No se pudo establecer conexion con el ACR" );
					return ERROR;
				}
				/*tSocket* sockDummy = (tSocket*)CrearSocket();
				sockDummy -> estado = estadoConn_standby;
				PCB.m_ListaSockets[SOCK_ADP] = sockDummy;*/
				
			}else if( PCB.nIdProcesoPadre = _ID_ADP_ )	/*si lo creo el ADP, se conecta con el ADP*/
			{
				if ( PCB_ConectarConADP() == ERROR )
				{
					Log_log( log_error, "No se pudo establecer conexion con el ADP" );
					return ERROR;
				}
				/*tSocket* sockDummy = (tSocket*)CrearSocket();
				sockDummy -> estado = estadoConn_standby;
				PCB.m_ListaSockets[SOCK_ACR] = sockDummy;*/				
			} 
		}
		
		signal(SIGALRM, PCB_ProcesarSeniales);
		signal(SIGCHLD, PCB_ProcesarSeniales);
		signal(SIGTERM, PCB_ProcesarSeniales);
		signal(SIGUSR1, PCB_ProcesarSeniales);
		signal(SIGUSR2, PCB_ProcesarSeniales);
		signal(SIGINT,  PCB_ProcesarSeniales);
	
		/*PCB_Migrar("127.0.0.1",9550, SOCK_ADP); /* Esto fue de prueba*/
		
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
		Log_printf(log_debug,"abriendo el archivo config");
		if ( !(cfg = config_Crear( "config", _ADP_ )) ) 
			break;	
	
		PCB.m_ADP_Port = config_GetVal_Int( cfg, _ADP_, "ADP_PORT" );
		
		if ( (tmp = config_GetVal( cfg, _ADP_ , "ADP_IP" ) ) )
		{
			strncpy( PCB.m_ADP_IP, tmp, LEN_IP );
		}
		config_Destroy(cfg);
		tmp = NULL;
	
		sprintf(strConfig, "config.ppcb%ld", PCB.PPCB_ID);
		
		Log_printf(log_debug,"abriendo el archivo %s",strConfig);
		if ( !(cfg = config_Crear( strConfig, _PPCB_ )) ) 
			break;
		
		
		
		if ( (tmp = config_GetVal( cfg, _PPCB_, "CREATORID" ) ) )
		{
			strncpy( PCB.User, tmp, LEN_USUARIO );
		}
		
		PCB.SessionID = config_GetVal_Int( cfg, _PPCB_, "SESIONID" );
		
		if ( (tmp = config_GetVal( cfg, _PPCB_, "COMANDO" ) ) )
		{
			strncpy( PCB.ProgName, tmp, LEN_COMANDO_EJEC );
		}
						
		sprintf(strCode, "CODE%d", line);
		
		while ( (tmp = config_GetVal( cfg, _PPCB_, strCode) ) ) {
			strncpy( PCB.Code[line-1], tmp, 50);
			
			line++;
			sprintf(strCode, "CODE%d", line);
			
		}
		PCB.ultimaSentencia = line-1;
		Log_printf(log_debug,"Config(LeerConfig): PCB.ultimaSentencia %d",PCB.ultimaSentencia);
		sscanf( PCB.Code[0], "MEM %d", &(PCB.Mem));	/*tomo la cantidad de memoria*/
		
		if ( (PCB.IP = config_GetVal_Int( cfg, _PPCB_, "IPOINTER" ) ) ){
					
		}
		
		if (!strcmp("PENDIENTE", config_GetVal( cfg, _PPCB_, "ESTADO")) ){
			PCB.State = PENDIENTE;
		} else if (!strcmp("BLOQUEADO", config_GetVal( cfg, _PPCB_, "ESTADO")) ) {
			PCB.State = BLOQUEADO;
		} else if (!strcmp("LISTO", config_GetVal( cfg, _PPCB_, "ESTADO")) ) {
			PCB.State = LISTO;
		} else if (!strcmp("EJECUTANDO", config_GetVal( cfg, _PPCB_, "ESTADO")) ) {
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
		
		PCB.tiempoRestanteOper = config_GetVal_Int( cfg, _PPCB_, "TMP_REST_OPER" );
		
		/*
		
		PCB.m_Port = config_GetVal_Int( cfg, _MSHELL_, "PCB_PORT" );
		*/
		/*PCB.m_nMemDisp = PCB.m_nMemMax = config_GetVal_Int( cfg, _PCB_, "MAX_MEM" );
		PCB.m_Q = config_GetVal_Int( cfg, _PCB_, "Q" );
		PCB.m_nLimite1 = config_GetVal_Int( cfg, _PCB_, "L1" );
		PCB.m_nLimite2 = config_GetVal_Int( cfg, _PCB_, "L2" );
		*/
		
		
		config_Destroy(cfg);
		
		/* PARA PRUEBA */
		/*PCB_ExecuteProgram();*/
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
	unsigned char szIP[4] = {'0','0','0','0'}; /* Por las dudas... estoy muy cansado y no se lo que hago.. ahora me voy al baño */
	
	memset( szIP, 0, 4 );
	/*ReducirIP( PCB.m_IP, szIP );*/
	
	
	if ( !( pSocket = conexiones_ConectarHost( PCB.m_ACR_IP, PCB.m_ACR_Port,
										 &PCB_ConfirmarConexion ) ) )
		return ERROR;

	PCB.m_ultimoSocket = 0;
	PCB.m_ListaSockets[ PCB.m_ultimoSocket ] = pSocket;
	PCB.m_socketACR = pSocket;
	
	/*Mando el Ping al ACR*/
	Log_log( log_debug, "envio Ping para conectarme con ACR" );
	
	if ( !(pPaq  = paquetes_newPaqPing(szIP, _ID_PPCB_, conexiones_getPuertoLocalDeSocket(pSocket) )) )
		return ERROR;
	
	memcpy(pPaq->msg,&PCB.PPCB_ID, sizeof(long)); /* Se le agrega el ID del PCB al mensaje del ping hacia el ACR, no queremos hacer otro paquete */
	
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	
	paquetes_destruir( pPaq );
	
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
	
	return ERROR;
}
/**********************************************************************/
int PCB_ConectarConADP()
{
	tSocket *pSocket;
	tPaquete *pPaq;
	int		nSend;
	unsigned char szIP[4];
	
	memset( szIP, 0, 4 );
	
	
	
	if ( !( pSocket = conexiones_ConectarHost( PCB.m_ADP_IP, PCB.m_ADP_Port,
										 &PCB_ConfirmarConexion ) ) )
		return ERROR;
	
	PCB.m_ultimoSocket = 0;
	PCB.m_ListaSockets[ PCB.m_ultimoSocket ] = pSocket;
	PCB.m_socketADP = pSocket;
	
	/*Mando el Ping al ADP*/
	Log_log( log_debug, "envio Ping para conectarme con ADP" );
	
	if ( !(pPaq  = paquetes_newPaqPing(szIP, _ID_PPCB_, conexiones_getPuertoLocalDeSocket(pSocket) )) )
		return ERROR;
	
	memcpy(pPaq->msg,&PCB.PPCB_ID, sizeof(long)); /* Se le agrega el ID del PCB al mensaje del ping hacia el ACR, no queremos hacer otro paquete */
	/*15-10-07: MA: lo anterior se lo pongo porque asi estaba en el ConectarConACR, talves el adp tambien lo usa o piensan usarlo*/
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	
	paquetes_destruir( pPaq );
	
	return (nSend == PAQUETE_MAX_TAM) ? OK: ERROR;
	
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
	} else 	if ( IS_ADP_PAQ( paq ) &&  IS_PAQ_PONG ( paq ) )
	{/*Si el ADP me responde pong la conexion queda establecida!*/
		Log_log( log_debug, "Conexion establecida con el ADP!" );
		sockIn->callback = &PCB_AtenderADP;
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
	int				nSend;
	unsigned char	szIpRed[4] = {'\0','\0','\0','\0'};
	unsigned char	szIpRed2[4] = {'\0','\0','\0','\0'};
	unsigned char	idProceso;
	char			szIp[LEN_IP];
	unsigned short 	nPuerto,nPuerto2;


	len = conexiones_recvBuff(sockIn, buffer, PAQUETE_MAX_TAM);
	
	if ( ERROR == len || !len)
	{
		Log_log( log_debug, "Se cae conexion con ACR" );
		PCB_CerrarConexion( sockIn );		
	
		return;
	}
	
	paq = paquetes_CharToPaq(buffer);

	if ( IS_PAQ_MIGRATE( paq) ) {
		/* MIGRAR HACIA EL ADP */
		Log_log(log_info,"Llega un PAQ_MIGRATE");
		
		paquetes_ParsearPaqMigrate(buffer,szIpRed2,&idProceso,&nPuerto2,szIpRed,&nPuerto);
		AmpliarIP(szIpRed,szIp);
		PCB_Migrar(szIp,nPuerto);
		
	}else if( IS_PAQ_MIGRAR_OK( paq ) ){
		Log_log(log_info,"Llega un PAQ_MIGRATE_OK");
		Log_log(log_info,"Kamikaze");
		PCB_Salir();	/*Si nacio en el nodo del ADP entonces luego de migrar debe cerrarse por si solo*/
		
	}
	
	if ( paq ) 
		paquetes_destruir( paq );
	
}


/**********************************************************/
void PCB_AtenderADP ( tSocket *sockIn )
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
		Log_log( log_debug, "Se cae conexion con ADP" );
		PCB_CerrarConexion( sockIn );
		if( PCB.State == EJECUTANDO ) PCB.State = LISTO;  /*Evito que siga ejecutando en el ACR*/
		PCB_Migrar(PCB.m_ACR_IP,PCB.m_ACR_Port);	/*Migra al ACR*/
		return;
	}
	
	paq = paquetes_CharToPaq(buffer);

	if ( IS_PAQ_MIGRAR_OK( paq ) )
	{/*La migracion concluyo con exito*/
		
		Log_log( log_info, "el ADP me manda un PAQ_MIGRAR_OK" );
		paq->id.id_Proceso = _ID_PPCB_;
		nSend = conexiones_sendBuff( PCB.m_socketACR, 
					(const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
		if(nSend == 0 || nSend == ERROR )
			Log_log(log_error,"Error al enviar PAQ_MIGRAR_OK");
			 
	} else if( IS_PAQ_MIGRAR_FAULT(paq) ){

		Log_log( log_info, "el ADP me manda un PAQ_MIGRAR_FAULT" );
		paq->id.id_Proceso = _ID_PPCB_;
		nSend = conexiones_sendBuff( PCB.m_socketACR, 
					(const char*) paquetes_PaqToChar( paq ), PAQUETE_MAX_TAM );
		if(nSend == 0 || nSend == ERROR )
			Log_log(log_error,"Error al enviar PAQ_MIGRAR_FAULT");		 
		
	} else  if( IS_PAQ_SUSPEND_PCB(paq) ){
		
		Log_log(log_info, "Se suspende la ejecucion del PPCB");
		PCB.State = LISTO;
		
	}else if ( IS_PAQ_EXEC_PCB(paq) ){
		if( PCB.State == LISTO ){
			Log_log(log_info, "Se reanuda la Ejecucion del PPCB");
			PCB.State = EJECUTANDO;
		}else{
			Log_log(log_info, "Se intento reanudar la Ejecucion del PPCB pero no tiene estado LISTO");
		}
	}else if (IS_PAQ_GET_REMAINING_TIME_EXECUTION(paq)){
		tPaquete *pPaq;
		int		nSend;
		unsigned char szIP[4];
		int tiempoRestante;
		
		memset( szIP, 0, 4 );
		
		if (ReducirIP(PCB.m_IP,szIP) == ERROR)
			return;
		if ((tiempoRestante = PCB_RemainingTimeExecution()) == ERROR)
			return;
		
		Log_log( log_info, "Enviando el tiempo restante de ejecucion al ADP");
		
		pPaq = paquetes_newPaqInfoRemainingTimeExecution(szIP, (unsigned char)_PPCB_, PCB.m_Port, PCB.PPCB_ID, tiempoRestante );
		nSend = conexiones_sendBuff( PCB.m_socketADP, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
		if ( nSend != PAQUETE_MAX_TAM )
		{
			Log_logLastError( "Error enviando INFO_REMAINING_TIME_EXECUTION al ADP" );
		}	
		paquetes_destruir( pPaq );
	}else if (IS_PAQ_SOL_CONCEDIDO(paq)){
		int id;
		memcpy( &id, &(paq->msg[SOLDEV_POS_PPCBID]), sizeof( int ) );
		Log_printf(log_info, "se concedio la solicitud al ppcb_id %d",id);
		PCB.State = LISTO;
	}
	if ( paq ) 
		paquetes_destruir( paq );
	
}


/**********************************************************/
int PCB_Migrar(char szIp[LEN_IP], unsigned short int nPuerto)
/*	Procedimiento de MIGRACION
 * El tercer parametro es la macro SOCK_ACR o SOCK_ADP, dependiendo el caso
 */
{
	tSocket 	*pSocket;
	tPaquete 	*pPaq;
	tPaqueteArch*pPaqLargo;
	int			nSend;
	unsigned char szIP[4] = {'0','0','0','0'};
	void		(*callback)(struct sSocket*);
	char* 		tmp; 
	int 		len, cantLeido, byteLeido; 
	char 		buffer[ PAQUETE_MAX_TAM ];
	char		parteArchivo[MAX_PAQ_ARCH];
	char		strBuff[50];
	FILE		*cfgFile=NULL;
	
	Log_log(log_info,"Inicia Procedimiento de migracion");
	
	createPCBConfig();
	Log_log(log_debug,"Cree el archivo de configuracion");
	
	/*[INI]	Conexion ----------------------------------------------*/
	Log_printf(log_debug,"Intento conectarme con ip:%s,puerto:%d",szIp,nPuerto);
	
	if( PCB.nIdProcesoPadre == _ID_ADP_ ){
		callback = PCB_AtenderACR;	/*si lo creo el ADP entonces se intentara conectar con el ACR al migrar*/
	}else{
		callback = PCB_AtenderADP;
	}

	if ( !( pSocket = conexiones_ConectarHost( szIp, nPuerto,callback ) ) )
		return ERROR;
	
	if( PCB.nIdProcesoPadre == _ID_ADP_ ){
		PCB.m_socketACR = pSocket;
	}else{
		PCB.m_socketADP = pSocket;
	}

	/*PCB.m_ListaSockets[ posSocket ] = pSocket;*/
	/*TODO: Pensar que asignarle al "ultimoSocket"*/
	/*PCB.m_ultimoSocket = SOCK_ADP; /*En todos los casos atiende a ambos sockets (creo)*/
	
	PCB.m_ListaSockets = realloc( PCB.m_ListaSockets,
			( ++PCB.m_ultimoSocket + MALLOC_SOCKS_INI ) * sizeof( tSocket* ) );

	PCB.m_ListaSockets[ PCB.m_ultimoSocket ] = pSocket;
	
	
	/*Mando el Ping*/
	Log_log( log_debug, "envio Ping para conectarme" );
	
	if ( !(pPaq  = paquetes_newPaqPing(szIP, _ID_PPCB_, conexiones_getPuertoLocalDeSocket(pSocket) )) )
		return ERROR;
	
	memcpy(pPaq->msg,&PCB.PPCB_ID, sizeof(long)); /* Se le agrega el ID del PCB al mensaje del ping hacia el ACR, no queremos hacer otro paquete */
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	
	if(pPaq)
		paquetes_destruir(pPaq);
	/*[FIN]	Conexion*/
	Log_log(log_debug,"Conexion exitosa");

	/*[INI] Confirmacion ----------------------------------------------*/
	len = conexiones_recvBuff( pSocket, buffer, PAQUETE_MAX_TAM );
	
	if ( ERROR == len || !len)
	{
		conexiones_CerrarSocket( PCB.m_ListaSockets, pSocket, &PCB.m_ultimoSocket );
		return ERROR;
	}
	
	pPaq = paquetes_CharToPaq( buffer );

	if ( IS_ACR_PAQ( pPaq ) &&  IS_PAQ_PONG ( pPaq ) )
	{/*Si el ACR me responde pong la conexion queda establecida!*/
		Log_log( log_debug, "Conexion establecida con el ACR!" );
		pSocket->callback = &PCB_AtenderACR;
	} else 	if ( IS_ADP_PAQ( pPaq ) &&  IS_PAQ_PONG ( pPaq ) )
	{/*Si el ADP me responde pong la conexion queda establecida!*/
		Log_log( log_debug, "Conexion establecida con el ADP!" );
		pSocket->callback = &PCB_AtenderADP;
	}
	
	if ( pPaq ) 
		paquetes_destruir( pPaq );
	/*[FIN] Confirmacion*/
	Log_log(log_debug,"Confirmacion exitosa");
	
	/*[INI] PAQ_MIGRAR ----------------------------------------------*/
	if ( !(pPaq  = paquetes_newPaqMigrar(szIP, _ID_PPCB_,
						conexiones_getPuertoLocalDeSocket(pSocket), PCB.PPCB_ID )) )
		return ERROR;
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqToChar( pPaq ), PAQUETE_MAX_TAM );
	if(nSend == 0 || nSend == ERROR )
		Log_log(log_error,"Error al enviar PAQ_MIGRAR");
	
	if ( pPaq ) 
		paquetes_destruir( pPaq );
	/*[FIN] PAQ_MIGRAR*/
	Log_log(log_debug,"Aviso migracion exitosa");
	
	/*[INI] PAQ_ARCHIVO ----------------------------------------------*/
	
	sprintf(strBuff, "config.ppcb%ld", PCB.PPCB_ID);
	if( !(cfgFile = fopen(strBuff, "rb")) ){
		Log_logLastError("Abriendo archivo config");
		return ERROR;
	}
	cantLeido = 0;
	while( (byteLeido = fgetc(cfgFile)) != EOF ){
		
		if( byteLeido != '\r' )
			parteArchivo[cantLeido++] = (char)byteLeido;
		
		if( cantLeido % MAX_PAQ_ARCH == 0 ){
			
			if( PCB_EnviarArchivo(parteArchivo,pSocket) == ERROR ){
				fclose(cfgFile);
				return ERROR;
			}
			bzero(parteArchivo,sizeof(parteArchivo));
			cantLeido = 0;
		}
	}
	
	if( cantLeido > MAX_PAQ_ARCH ){
		Log_log(log_error,"Error se leyo mas del maximo");
	}else if( cantLeido > 0 ){
		while( cantLeido < MAX_PAQ_ARCH )
			parteArchivo[cantLeido++] = '\0';	/*Completo con NULL*/
	}
	
	if( PCB_EnviarArchivo(parteArchivo,pSocket) == ERROR ){
		fclose(cfgFile);
		return ERROR;
	}
	fclose(cfgFile);
	/*[FIN] PAQ_ARCHIVO*/
	Log_log(log_debug,"Envio archivo exitoso");
	
	/*[INI] PAQ_FIN_MIGRAR ----------------------------------------------*/
	if( PCB_EnviarFinMigrar(pSocket) == ERROR ){
		return ERROR;
	}
	/*[FIN] PAQ_FIN_MIGRAR*/
	Log_log(log_debug,"Envio fin migracion exitoso");
	
	return OK;
}

/**********************************************************/
int PCB_EnviarArchivo( char parteArchivo[MAX_PAQ_ARCH], tSocket* pSocket){
	tPaqueteArch	*pPaqLargo;
	int				nSend;
	unsigned char 	szIP[4] = {'0','0','0','0'};

	if ( !(pPaqLargo = paquetes_newArchivo(szIP, _ID_PPCB_,
						conexiones_getPuertoLocalDeSocket(pSocket), parteArchivo )) ){
		Log_log(log_error,"Error al crear PAQ_ARCHIVO");
		return ERROR;
	}
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqArchToChar( pPaqLargo ),
									PAQUETE_ARCH_MAX_TAM );
	if(nSend == 0 || nSend == ERROR ){
		Log_logLastError("Error al enviar PAQ_ARCHIVO");
		if ( pPaqLargo ) 
			paquetes_Archdestruir( pPaqLargo );
		return ERROR;
	}
	
	if ( pPaqLargo ) 
		paquetes_Archdestruir( pPaqLargo );
	return OK;
}
/**********************************************************/
int PCB_EnviarFinMigrar( tSocket* pSocket ){
	tPaqueteArch	*pPaqLargo;
	int				nSend;
	unsigned char 	szIP[4] = {'0','0','0','0'};

	if ( !(pPaqLargo = paquetes_newPaqFinMigrar(szIP, _ID_PPCB_,
						conexiones_getPuertoLocalDeSocket(pSocket) )) ){
		Log_log(log_error,"Error al crear PAQ_FIN_MIGRAR");
		return ERROR;
	}
	
	memcpy( &(pPaqLargo-> id.UnUsed[0]), &(PCB.Mem), sizeof(int) );
	memcpy( &(pPaqLargo-> msg), &(PCB.State), sizeof(int) );
	
	nSend = conexiones_sendBuff( pSocket, (const char*) paquetes_PaqArchToChar( pPaqLargo ),
									PAQUETE_ARCH_MAX_TAM );
	if(nSend == 0 || nSend == ERROR ){
		Log_log(log_error,"Error al enviar PAQ_FIN_MIGRAR");
		if ( pPaqLargo ) 
			paquetes_Archdestruir( pPaqLargo );
		return ERROR;
	}
	
	if ( pPaqLargo ) 
		paquetes_Archdestruir( pPaqLargo );
	return OK;
}

/**********************************************************/
void PCB_Salir()
{
	/* Logear salida, hacer un clean up? */
	
	conexiones_CerrarLista( 0, &PCB.m_ultimoSocket, PCB.m_ListaSockets );
	
	Log_log(log_info,"Fin de la ejecucion");
	/*pantalla_Clear(); */
	exit(EXIT_SUCCESS);
}

/**********************************************************/
void 	PCB_CerrarConexion( tSocket *sockIn )
{/*MIGRAR AL ACR*/
	/*11/10/2007	GT	TODO: verificar porque hay que conservar los lugares de los sockets*/
	conexiones_CerrarSocket( PCB.m_ListaSockets, sockIn, &PCB.m_ultimoSocket );
}

/**********************************************************/
int		PCB_RecursoFromFriendlyName(tRecurso* recurso, const char* param)
{
	int result = ERROR;
	if ((strcmp("Impresora", param) == 0) || (strcmp("impresora", param) == 0))
	{
		*recurso = Impresora;
		result = OK;
	}
	else if((strcmp("Disco", param) == 0) || (strcmp("disco", param) == 0))
	{
		*recurso = Disco;
		result = OK;
	}
	else if ((strcmp("Cinta", param) == 0) || (strcmp("cinta", param) == 0))
	{
		*recurso = Cinta;
		result = OK;
	}
	return result;
}
/**********************************************************/
int PCB_RemainingTimeExecution()
{
	int line = PCB.IP;
	int remainingTime = 0;
	
	Log_log(log_info, "Calculando tiempo restante de ejecucion");
	
	while (line <= PCB.ultimaSentencia)
	{
		char sentence[50];
		char instruction[5];
		int paramStart;
		
		strncpy(sentence, PCB.Code[line], 50);
		
		strncpy(instruction, sentence, 4);
		instruction[4] = '\0';
		
		if ( !strcmp("MEM ", instruction) )
		{
			line++;
			continue;
		}
		
		if ( !strcmp("", instruction) )
		{
			break;
		}
				
		if ( !strcmp("SOL ", instruction) 	||
		     !strcmp("DEV ", instruction) 	||
			 !strcmp("IMP ", instruction) 	||
		     !strcmp("PUSH", instruction) 	||
			 !strncmp("POP", instruction, 3) 
		   ) 
		{
			remainingTime += 1;	
		} 
		else if ( !strcmp("OPER", instruction) ) 
		{
			int tiempoOper;
			
			if ( sentence[3] == ' ' ) {
				paramStart = 4;
			} else if ( sentence[4] == ' ') {
				paramStart = 5;
			}
			tiempoOper = atoi(sentence+paramStart);
			
			remainingTime += tiempoOper;
		}  
		else 
		{
			Log_log( log_error, "Error al calcular el tiempo restante de ejecucion, checkee el script");
			return ERROR;
		}
		line++;
	}
	return remainingTime;
}
/**********************************************************/
void SetRemainingTime(int remainingTimeExecution)
{
		tConfig *cfg;
		char strBuff[50];
		char strVal[10];
			
		do {
			sprintf(strBuff, "config.ppcb%ld", PCB.PPCB_ID);
			
			if( ExistPath(strBuff) != ERROR && PCB.argc == 3 )
			{	/*existe el archivo*/
			
				if ( !(cfg = config_Crear( strBuff, _PPCB_ )) ) 
					break;	
				
				bzero(strVal,sizeof(strVal));

				sprintf(strVal,"%d",remainingTimeExecution);
				config_SetVal(cfg,_PPCB_,"TMP_REST",strVal);
				
				config_Guardar(cfg,strBuff);
				
				config_Destroy(cfg);
				
			}
		} while (0);	
}
/**********************************************************/
void PCB_ImprimirInfoCtr()
{
	int line = 0;
	int i = 0;
	Stack* stack = NULL;
	
	Log_log(log_info, "  Se imprime la informacion de control del PPCB  ");
	
	InfoCtr_Inicializar("PPCB","1");
	
	InfoCtr_printf(log_info, "Usuario: %s", PCB.User);
	InfoCtr_printf(log_info, "Comando completo: exec %s", PCB.ProgName);
	/*code*/
	InfoCtr_log(log_info, "Codigo del PPCB:");
	while( line < PCB.ultimaSentencia ) {
		InfoCtr_log(log_info, PCB.Code[line]);
		line++;
	}
	InfoCtr_printf(log_info, "Instruction pointer: %d", PCB.IP);
	/*stack*/
	InfoCtr_log(log_info, "Contenido del stack:");
	stack = &(PCB.stack);
	if (stack->top == 0)
		InfoCtr_log(log_info, "El stack esta vacio");
	else
	{
		for (i=0;i<stack->top;i++)
		{
			InfoCtr_printf(log_info, "posicion %d: %c ", i, stack->v[i]);
		}
	}

	
	InfoCtr_CerrarInfo();

}
/**********************************************************/
void	PCB_VolverAlACR(tSocket *sockIn)
{
	Log_log( log_warning, "El ADP esta caido, se migra al ACR");
	if( PCB.State == EJECUTANDO ) PCB.State = LISTO;  /*Evito que siga ejecutando en el ACR*/
	PCB_Migrar(PCB.m_ACR_IP,PCB.m_ACR_Port);
	return;
}
/**********************************************************/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
