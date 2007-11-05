#include "infoControl.h"

extern int errno;

static char g_szProgramai[30];
static char g_nivelMinimoi;
static FILE* fpi;

/* Prototipos privados */
static char InfoCtr_str2nivel(const char *nivel);
static char* InfoCtr_nivel2str(char nivel);

/*---------------------------------------------------------*/
void InfoCtr_Inicializar(const char *pszProg, const char* nivelMinimo)
{
	char		szPathLog[512];
	time_t		stTime = time( NULL );
	struct tm*	stHoy = localtime( &stTime );

	sprintf( szPathLog, "%04d%02d%02d-InfoCtr-%s.log", stHoy->tm_year+1900, stHoy->tm_mon+1, stHoy->tm_mday, pszProg );

	if( ( fpi = fopen( szPathLog, "a" ) ) == NULL )
		printf("Error al abrir el archivo de info\n");
	else
		InfoCtr_log(log_info_ctr, "\nInicio de ejecucion.");
		
	strncpy( g_szProgramai, pszProg, sizeof g_szProgramai );
	g_nivelMinimoi = InfoCtr_str2nivel(nivelMinimo);	
}

const char* InfoCtr_GetNivelLog()
{
	return InfoCtr_nivel2str(g_nivelMinimoi);
}

/*---------------------------------------------------------*/
static char* InfoCtr_nivel2str(char nivel)
{
	switch(nivel)
	{
		case log_error_ctr:		return "Error";
		case log_warning_ctr:	return "Warning";
		case log_info_ctr:		return "Info";
		case log_debug_ctr:		return "Debug";
	}
	
	return "WTF";
}
static char InfoCtr_str2nivel(const char *nivel)
{
	if (nivel){
		if (!strcasecmp(nivel, "error")) return log_error_ctr;
		if (!strcasecmp(nivel, "warning")) return log_warning_ctr;
		if (!strcasecmp(nivel, "info")) return log_info_ctr;
	}
	return log_debug_ctr;
}

/*---------------------------------------------------------*/
void InfoCtr_log( char nivel, const char *pszMsj )
{
	time_t		stTime = time( NULL );
	struct tm*	stHoy = localtime( &stTime );

	if (nivel < g_nivelMinimoi) return;
	
  	fprintf( fpi, "%02d:%02d:%02d [PID %d] %s: %s\n", 
		stHoy->tm_hour, stHoy->tm_min , stHoy->tm_sec, getpid(), InfoCtr_nivel2str(nivel), pszMsj);
		
	fflush(fpi);
}

void InfoCtr_printf( char nivel, const char* fmt, ... )
{
	va_list args;	
	char buffer[500];
	
	if (nivel < g_nivelMinimoi) return;
	
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);
	InfoCtr_log(nivel, buffer);
}

/*---------------------------------------------------------*/
void InfoCtr_logLastError( const char* msg )
{
	char buffer[500];
	
	if (errno != 0){
		sprintf(buffer, "%s: (%d) %s", msg, errno, strerror( errno ));
		InfoCtr_log( log_error_ctr, buffer );
	}

}

void InfoCtr_CerrarInfo()
{
	if ( fclose(fpi) )
		printf("Error al cerrar el archivo de info\n");
}
