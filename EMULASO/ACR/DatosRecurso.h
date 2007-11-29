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
#define MAX_LISTA_BLOQ			15


/* Los datos de un Recurso */
typedef struct
{
	char 			szNombre[ LARGO_NOMBRE_REC ];
	int 			nInstancias;	/* R: cantidad total de instancias del recurso */
	int				nSemaforo;		/* S: indica el comportamiento símil semáforo */
	int				nAvailable;		/* A: cantidad de instacias disponibles */
	long			ListaBloqueados[MAX_LISTA_BLOQ +1];	/*los id de los bloqueados por este recurso en FIFO*/
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

int 				Rec_IncrementarInst( tDatosRecurso *pE, const int bConLiberacion );
int 				Rec_DecrementarInst( tDatosRecurso *pE, const int bConAsignacion );
												
char				Rec_EstanLosRecursos( tDatosRecurso *pLista, const int nTope, char* szRecursos );
int 				Rec_ConvertirVect( tDatosRecurso *pLista, const char* szRecursos,
						const int nTope, int *pInstancias );
void				Rec_ObtenerVectorDisponibles( tDatosRecurso *pLista, const int nTope, int* pInstancias );

int					Rec_AgregarBloqueado(tDatosRecurso* recurso, long ppcbid);
long				Rec_ObtenerBloqueado(tDatosRecurso* recurso, int pos);
long 				Rec_EliminarPidDeBloqueados(tDatosRecurso* recurso, long pid);
long				Rec_QuitarBloqueado(tDatosRecurso* recurso);
int 				Rec_SinBloqueados(tDatosRecurso* recurso);

#endif /*DATOSRECURSO_H_*/

/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
