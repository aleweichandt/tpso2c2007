/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: FindFile.h
 * 
 * Historia
 *	15-04-2007	GT	Creacion
 * 	17/07/2007	GT	Se agrega funcion para la existencia de un path
 *
 *------------------------------------------------------------------ 
 */
#ifndef FINDFILE_H_
#define FINDFILE_H_

int ExistFile(const char*szPath,const char*szFile);

int ExistPath(const char *szPath);

long FileSize(const char*szPath,const char*szFile);

float FileSizeKBytes(const char*szPath,const char*szFile);

#endif /*FINDFILE_H_*/

/*---------------------<Fin de Archivo >----------------------------------*/
