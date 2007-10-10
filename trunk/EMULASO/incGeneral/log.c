#include "log.h"

extern int errno;

static char g_szPrograma[30];
static char g_nivelMinimo;
static FILE* fp;

/* Prototipos privados */
static char log_str2nivel(const char *nivel);
static char* log_nivel2str(char nivel);

/*---------------------------------------------------------*/
void Log_Inicializar(const char *pszProg, const char* nivelMinimo)
{
	char		szPathLog[512];
	time_t		stTime = time( NULL );
	struct tm*	stHoy = localtime( &stTime );

	sprintf( szPathLog, "%04d%02d%02d-%s.log", stHoy->tm_year+1900, stHoy->tm_mon+1, stHoy->tm_mday, pszProg );

	if( ( fp = fopen( szPathLog, "a" ) ) == NULL )
		printf("Error al abrir el archivo de log\n");
	else
		Log_log(log_info, "\nInicio de ejecucion.");
		
	strncpy( g_szPrograma, pszProg, sizeof g_szPrograma );
	g_nivelMinimo = log_str2nivel(nivelMinimo);	
}

const char* log_GetNivelLog()
{
	return log_nivel2str(g_nivelMinimo);
}

/*---------------------------------------------------------*/
static char* log_nivel2str(char nivel)
{
	switch(nivel)
	{
		case log_error:		return "Error";
		case log_warning:	return "Warning";
		case log_info:		return "Info";
		case log_debug:		return "Debug";
	}
	
	return "WTF";
}
static char log_str2nivel(const char *nivel)
{
	if (nivel){
		if (!strcasecmp(nivel, "error")) return log_error;
		if (!strcasecmp(nivel, "warning")) return log_warning;
		if (!strcasecmp(nivel, "info")) return log_info;
	}
	return log_debug;
}

/*---------------------------------------------------------*/
void Log_log( char nivel, const char *pszMsj )
{
	time_t		stTime = time( NULL );
	struct tm*	stHoy = localtime( &stTime );

	if (nivel < g_nivelMinimo) return;
	
  	fprintf( fp, "%02d:%02d:%02d [PID %d] %s: %s\n", 
		stHoy->tm_hour, stHoy->tm_min , stHoy->tm_sec, getpid(), log_nivel2str(nivel), pszMsj);
		
	fflush(fp);
}

void Log_printf( char nivel, const char* fmt, ... )
{
	va_list args;	
	char buffer[500];
	
	if (nivel < g_nivelMinimo) return;
	
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);
	Log_log(nivel, buffer);
}

/*---------------------------------------------------------*/
void Log_logLastError( const char* msg )
{
	char buffer[500];
	
	if (errno != 0){
		sprintf(buffer, "%s: (%d) %s", msg, errno, strerror( errno ));
		Log_log( log_error, buffer );
	}

}

void Log_Cerrar(){
	fclose(fp);	
}
