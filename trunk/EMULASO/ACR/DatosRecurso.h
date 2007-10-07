/*
 *<<---------------- KERNEL PANIC ATTACK ----------------------------->>
 * Archivo: DatosRecurso.h
 * 
 * Historia
 *	23/09/2007	GT	Creacion
 *
 ------------------------------------------------------------------ 
 */

#ifndef DATOSRECURSO_H_
#define DATOSRECURSO_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../incGeneral/incGeneral.h"


#define LARGO_NOMBRE_REC		11
#define MAX_LISTA_REC			3	/*Hay 3 recursos: Impresora, Disco y Cinta*/


/* Los datos de un Recurso */
typedef struct
{
	char 			szNombre[ LARGO_NOMBRE_REC ];
	int 			nInstancias;	/* R: cantidad total de instancias del recurso */
	int				nSemaforo;		/* S: indica el comportamiento símil semáforo */
	int				nAvailable;		/* A: cantidad de instacias disponibles */
						
}tDatosRecurso;

/* Funciones Publicas */
void 				Rec_InicializarLista( tDatosRecurso *pLista, int *pnTope  );

int 				Rec_Agregar( tDatosRecurso *pLista, const char* szNombre, 
						int nInstancias, int *pnTope  );
/*int 				Rec_Eliminar( tDatosRecurso *pLista, const char* szNombre, 
						int nInstancias, int *pnTope  );*/
tDatosRecurso* 		Rec_Buscar( tDatosRecurso *pLista, 
						const char* szNombre, const int nTope, int* pnPos  );
tDatosRecurso* 		Rec_BuscarXPos( tDatosRecurso *pLista, const int nTope, const int nPos );

int 				Rec_IncrementarInst( tDatosRecurso *pLista, 
						const char* szNombre, const int pnTope, tDatosRecurso *pE, const int bConLiberacion );
int 				Rec_IncrementarInstXPos( tDatosRecurso *pLista, const long lTid, 
							const int pnTope, const int nPos, tDatosRecurso *pE, const int bConLiberacion  );
int			 		Rec_DecrementarInst( tDatosRecurso *pLista, 
						const char* szNombre, const int pnTope, tDatosRecurso *pE, const int bConAsignacion );
int 				Rec_DecrementarInstXPos( tDatosRecurso *pLista, const char* szNombre, 
							const int pnTope, const int nPos, tDatosRecurso *pE, const int bConAsignacion );
												
char				Rec_EstanLosRecursos( tDatosRecurso *pLista, const int nTope, char* szRecursos );
int 				Rec_ConvertirVect( tDatosRecurso *pLista, const char* szRecursos,
						const int nTope, int *pInstancias );
void				Rec_ObtenerVectorDisponibles( tDatosRecurso *pLista, const int nTope, int* pInstancias );

#endif /*DATOSRECURSO_H_*/

/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
