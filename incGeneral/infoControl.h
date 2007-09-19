#ifndef INFOCONTROL_H_
#define INFOCONTROL_H_

#include "incGeneral.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

enum log_nivel_ctr{
	log_debug_ctr=1,
	log_info_ctr, 
	log_warning_ctr, 
	log_error_ctr
};

void InfoCtr_Inicializar(const char *pszProg, const char *nivelMinimo);
void InfoCtr_log( char nivel, const char *pszMsj );
const char* InfoCtr_GetNivelLog();
void InfoCtr_printf( char nivel, const char* fmt, ... );
void InfoCtr_logLastError( const char* msg );
void InfoCtr_CerrarInfo();
#endif /*INFOCONTROL_H_*/
