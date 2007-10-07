/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: FindFile.c
 * 
 * Historia
 *	15-04-2007	GT	Creacion
 *  17/07/2007	GT	Se agrega la funcion para la existencia de un path
 *
 *------------------------------------------------------------------ 
 */
/**
 * FindFile
 * Es el encargado de verificar la existencia de un archivo en un directorio. 
 * 
 */

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "FindFile.h"

#define STR_BARRA "/"
#define BARRA '/'
#define BARRA_INCL '\\'
#define DOS_ALA_DIEZ 1024

/*****************************************************************/
/**
 * Dados un path y un nombre de archivo, verifica la existencia del mismo en el
 * directorio especificado.
 * 
 * Retorna 0 si existe el archivo y -1 para otro caso.
 */
int ExistFile(const char*szPath,const char*szFile){
	DIR * dir;
	struct dirent * direntity;
	char * pathfound;
	
	if( ( dir = (DIR*)opendir(szPath) ) == NULL ){
		/*ERROR*/
		return -1;
	}
	
	while ( ( direntity = (struct dirent*)readdir(dir) ) != NULL){
		pathfound = direntity->d_name;
		
		if( strcmp(pathfound,szFile) == 0 )
		{
			closedir(dir);/*20-06-07:LAS: Le doy un close porsi aloco mem, ya que el valgrind se queja!*/
			return 0; /*Exito: Si el nombre del archivo del directorio es igual al del parametro*/
		}
	}
	/*No se encontró el archivo o hubo un error*/
	closedir(dir);
	
	return -1;
}

/*****************************************************************/
int ExistPath(const char *szPath)
{
	return access(szPath,F_OK);
}
/*****************************************************************/
/**
 * Dados un path y un nombre de archivo, retorna el tamaño del archivo en bytes
 * El path no necesita terminar con barra, pero puede estarlo.
 * Precondiciones: El archivo debe existir en el directorio especificado.
 */
long FileSize(const char*szPath,const char*szFile){
	struct stat infofile; /*contiene información del archivo*/ /*15-04-2007 GT Cambio a memoria estática*/
	char strbuf[strlen(szPath)+strlen(szFile)+2];
	int ultChar;
	
	strcpy(strbuf,szPath);
	ultChar = szPath[strlen(szPath)-1];
	if( ultChar != BARRA && ultChar != BARRA_INCL )
		strcat(strbuf,STR_BARRA);	
	stat(strcat(strbuf,szFile),&infofile);
	return infofile.st_size; /*contiene el tamaño del archivo*/
}
/*****************************************************************/
/**
 * Dados un path y un nombre de archivo, retorna el tamaño del archivo en kilobytes
 * El path no necesita terminar con barra, pero puede estarlo.
 * Precondiciones: El archivo debe existir en el directorio especificado.
 * Ver: FileFind - int FileSize(const char*szPath,const char*szFile)
 */ 
float FileSizeKBytes(const char*szPath,const char*szFile){
	long sizeBytes = FileSize(szPath,szFile);
	return (float)sizeBytes/ (float)DOS_ALA_DIEZ; 
}

/*---------------------<Fin de Archivo >----------------------------------*/
