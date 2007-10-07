/*
 *<<---------------- KERNEL PANIC ATTACK ----------------------------->>
 * Archivo: MatrizRecursos.h
 * 
 * Historia
 *	23/09/2007	GT	Creacion
 *
 ------------------------------------------------------------------ 
 */
#ifndef MATRIZRECURSOS_H_
#define MATRIZRECURSOS_H_

#include "../incGeneral/incGeneral.h"
#include "../incGeneral/Estructuras/estructuras.h"
#include "DatosRecurso.h"
#include "DatosPpcbACR.h"

typedef struct sFila{
	long tid;
	int valores[MAX_LISTA_REC];
} tFila;

#define tListaFilas t_nodo*

int	 	MatrizRec_Iniciar			(tListaFilas *lista, const int cantRecursos);
tFila*	MatrizRec_AgregarProceso	(tListaFilas *lista, const int cantRecursos, const long lTid,
									const int pInstanciasV[MAX_LISTA_REC] );
int  MatrizRec_EliminarProceso	(tListaFilas *lista, const int cantRecursos,	const long lTid);
int  MatrizRec_EliminarMatriz	(tListaFilas *lista);
int  MatrizRec_SumarInstancia	(tListaFilas *lista, const int cantRecursos, const long lTid,
									const int posRecurso, const int cantidad);
									
int	 MatrizRec_ObtenerInstancia		 (tListaFilas *lista, const int cantRecursos, const long lTid,
										const int posRecurso);
int* MatrizRec_ObtenerVectorInstancia(tListaFilas *lista, const int cantRecursos, const long lTid);
#endif /*MATRIZRECURSOS_H_*/
