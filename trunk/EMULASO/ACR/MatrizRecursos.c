/*
 *<<---------------- KERNEL PANIC ATTACK ----------------------------->>
 * Archivo: MatrizRecursos.h
 * 
 * Historia
 *	23/09/2007	GT	Creacion
 *
 ------------------------------------------------------------------ 
 */

#include "MatrizRecursos.h"

/*Funciones Privadas*/
int compararFila( const void* f1, const void* f2);

/*******************************************************************/
int	MatrizRec_Iniciar(tListaFilas *lista, const int cantRecursos)
{
	return lista_inic(lista);
}

/*******************************************************************/
tFila*	MatrizRec_AgregarProceso	(tListaFilas *lista, const int cantRecursos, const long lTid, const int pInstanciasV[MAX_LISTA_REC] )
{
	void *respuesta;
	tFila* filaTrf= malloc (sizeof(tFila));
	filaTrf->tid = lTid;
	memcpy ( &(filaTrf->valores), pInstanciasV, MAX_LISTA_REC* sizeof(int) );
	
	respuesta = lista_insertar(lista,filaTrf,sizeof(tFila),compararFila,_SIN_REPET_) ;
	return (respuesta == NULL)? NULL: (tFila *)respuesta;
}

/*******************************************************************/
int MatrizRec_EliminarProceso	(tListaFilas *lista, const int cantRecursos, const long lTid)
{
	tFila fila;
	fila.tid = lTid;
	return lista_quitar(lista,&fila,compararFila);
}

/*******************************************************************/
int MatrizRec_EliminarMatriz	(tListaFilas *lista)
{
	lista_destruir(lista);
}

/*******************************************************************/
int MatrizRec_SumarInstancia	(tListaFilas *lista, const int cantRecursos, const long lTid,
									const int posRecurso, const int cantidad)
{
	tFila* fila ;
	tFila filaBuscada;
	tListaFilas encontrado;
	
	filaBuscada.tid = lTid;
	if( !(encontrado = lista_buscar(lista,&filaBuscada,compararFila)) )	/*30/06/2007	GT	Pregunta si lo encuentra*/
		return ERROR;

	if ( cantRecursos >= posRecurso ){
		fila = (tFila*) encontrado->datos;
		fila->valores[posRecurso] += cantidad;
	}
}

/*******************************************************************/
int MatrizRec_ObtenerInstancia	(tListaFilas *lista, const int cantRecursos, const long lTid,
									const int posRecurso)
{
	tFila* fila ;
	tFila filaBuscada;
	tListaFilas encontrado;
	
	filaBuscada.tid = lTid;
	if( !(encontrado = lista_buscar(lista,&filaBuscada,compararFila)) )
		return ERROR;

	if ( cantRecursos >= posRecurso ){
		fila = (tFila*) encontrado->datos;
		return fila->valores[posRecurso];
	}
	return 99999;
}

/*******************************************************************/
int* MatrizRec_ObtenerVectorInstancia(tListaFilas *lista, const int cantRecursos, const long lTid)
{
	unsigned int alpe; 
	tFila* fila ;
	tFila filaBuscada;
	tListaFilas encontrado;
	
	filaBuscada.tid = lTid;
	if( !(encontrado = lista_buscar(lista,&filaBuscada,compararFila)) )
		return NULL;

	fila = nodo_datos(encontrado,&alpe);	/*01/07/2007	GT	cambio por nodo_datos*/
	return fila->valores;
}

/*******************************************************************/
int compararFila( const void* f1, const void* f2)
{
	tFila 	*fila1 = (tFila*) f1,
			*fila2 = (tFila*) f2;
	
	return (int)(fila1->tid -fila2->tid);
}
