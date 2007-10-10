#ifndef LOG_H
#define LOG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

enum log_nivel{
	log_debug=1,
	log_info, 
	log_warning, 
	log_error
};

void Log_Inicializar(const char *pszProg, const char *nivelMinimo);
void Log_log( char nivel, const char *pszMsj );
const char* log_GetNivelLog();
void Log_printf( char nivel, const char* fmt, ... );
void Log_logLastError( const char* msg );
void Log_Cerrar();

#endif
