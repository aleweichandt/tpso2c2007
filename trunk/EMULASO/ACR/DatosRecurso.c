/*
 *<<---------------- KERNEL PANIC ATTACK ----------------------------->>
 * Archivo: DatosRecurso.h
 * 
 * Historia
 *	23/09/2007	GT	Creacion
 *
 ------------------------------------------------------------------ 
 */
 
#include "DatosRecurso.h"

/***********************************************************************************/ 
void Rec_InicializarLista( tDatosRecurso *pLista, int *pnTope  )
{
	memset( pLista, '\0', sizeof( *pLista ) * MAX_LISTA_REC );
	*pnTope = 0;
}

/***********************************************************************************/

int Rec_Agregar( tDatosRecurso *pLista, const char* szNombre, int nInstancias, 
					int *pnTope  )
{
	int i = (*pnTope)++;

/*	for ( i = 0; i < MAX_LISTA_REC; i++ )
	{	
		if ( (pLista + i)->Estado == EstadoE_Libre || 
			(pLista + i)->Estado == EstadoE_Borrado )
		{*/
			strncpy( (pLista + i )->szNombre, szNombre, LARGO_NOMBRE_REC );
			(pLista + i )->nInstancias = nInstancias;
			(pLista + i )->nAvailable = nInstancias;
			(pLista + i )->nSemaforo = nInstancias;
			bzero( (pLista + i )->ListaBloqueados, sizeof((pLista + i )->ListaBloqueados));
/*			break;
		}
	}
	*/
	return (*pnTope);
}


/***********************************************************************************/

tDatosRecurso* Rec_Buscar( tDatosRecurso *pLista, const char* szNombre, 
									const int nTope, int* pnPos )
{
	
	for ( (*pnPos) = 0; (*pnPos) < nTope; (*pnPos)++ )
	{
		if ( strcmp( (pLista + (*pnPos))->szNombre, szNombre ) == 0 )
			return (pLista + (*pnPos));
	}
	
	return NULL;
}

/***********************************************************************************/
tDatosRecurso* Rec_BuscarXPos( tDatosRecurso *pLista, const int nTope, const int nPos )
{
	
	if (nPos < nTope )
		return (pLista + nPos);
	
	return NULL;
}

/***********************************************************************************/
/*
int Rec_Eliminar( tDatosRecurso *pLista, const char* szNombre, int nInstancias, 
						int *pnTope  )
						
{
	tDatosRecurso* pE;
	
	if ( !(pE = Rec_Buscar( pLista, szNombre, pnTope )) )
		return -1;
	
	pE->Estado = EstadoE_Borrado;
	strcpy( pE->szNombre, "" );
	
	(*pnTope)--;
}
*/

/***********************************************************************************/
int Rec_IncrementarInst( tDatosRecurso *pLista, const char* szNombre, 
							const int pnTope, tDatosRecurso *pE, const int bConLiberacion )
/*15/06/2007	GT	Se incrementan por su disponibilidad, nunca por su total */
{
	int i;
	
	if ( !(pE = Rec_Buscar(pLista, szNombre, pnTope, &i )) )
		return ERROR;
	
	if( bConLiberacion && pE->nAvailable < pE->nInstancias )		/*15/06/2007	GT	para corregir defasaje */
		++(pE->nAvailable);
	++(pE->nSemaforo);
	
/*	if ( pE->nInstancias > 0 )
		pE->Estado = EstadoE_ConDisponibilidad;
	else if ( pE->nInstancias == 0 ) /*Ya lo incremento en el if anterior*/
/*		pE->Estado = EstadoE_Agotado;*/
	
	return pE->nAvailable;
}

/***********************************************************************************/
int Rec_IncrementarInstXPos( tDatosRecurso *pLista, const long lTid, 
							const int pnTope, const int nPos, tDatosRecurso *pE, const int bConLiberacion )
/*15/06/2007	GT	Se incrementan por su disponibilidad, nunca por su total */
{

	if ( !(pE = Rec_BuscarXPos(pLista, pnTope, nPos )) )
		return ERROR;
	
	if( bConLiberacion && pE->nAvailable < pE->nInstancias )		/*15/06/2007	GT	para corregir defasaje */
		++(pE->nAvailable);
	++(pE->nSemaforo);
	
/*	if ( pE->nInstancias > 0 )
		pE->Estado = EstadoE_ConDisponibilidad;
	else if ( pE->nInstancias == 0 ) /*Ya lo incremento en el if anterior*/
/*		pE->Estado = EstadoE_Agotado;*/
	
	return pE->nAvailable;
}

/***********************************************************************************/
int Rec_DecrementarInst( tDatosRecurso *pLista, const char* szNombre, 
							const int pnTope, tDatosRecurso *pE, const int bConAsignacion )
{
	int i;
	int bResultado= FALSE;
			
	if ( !(pE = Rec_Buscar(pLista, szNombre, pnTope, &i )) )
		return ERROR;

	if( bConAsignacion && pE->nAvailable > 0 )		/*15/06/2007	GT	para corregir defasaje */
	{
		--(pE->nAvailable);
		bResultado = TRUE;
	}
	--(pE->nSemaforo);
	
/*	if ( --(pE->nInstancias) < 0 )
		pE->Estado = EstadoE_EnDeuda;
	else if ( pE->nInstancias == 0 ) /*Ya lo incremento en el if anterior*/	
/*		pE->Estado = EstadoE_Agotado;*/
	
	return bResultado;	/*04/07/2007	GT	TODO: OJO aca ¿que hice poner? -> verificar*/ /*17/07/2007	GT	Bug: Se modifico*/
}

/***********************************************************************************/
int Rec_DecrementarInstXPos( tDatosRecurso *pLista, const char* szNombre, 
							const int pnTope, const int nPos, tDatosRecurso *pE, const int bConAsignacion )
{
	int bResultado = FALSE;
	if ( !(pE = Rec_BuscarXPos(pLista, pnTope, nPos )) )
		return ERROR;

	if( bConAsignacion && pE->nAvailable > 0 )		/*15/06/2007	GT	para corregir defasaje */
	{
		--(pE->nAvailable);
		bResultado = TRUE;
	}
	--(pE->nSemaforo);
	
/*	if ( --(pE->nInstancias) < 0 )
		pE->Estado = EstadoE_EnDeuda;
	else if ( pE->nInstancias == 0 ) /*Ya lo incremento en el if anterior*/	
/*		pE->Estado = EstadoE_Agotado;*/
	
	return bResultado;
}

/***********************************************************************************/
char Rec_EstanLosRecursos( tDatosRecurso *pLista, const int nTope, char* szRecursos )
/*15/06/2007	GT	Verifica la exitencia de los recursos que me están pidiendo
 * independientemente de si tenga o no instancias disponibles*/
/* TODO: degub */
{
	char szEncrips[256];
	char *pAux;
	int	 nContSep = ContarCharEnString( szRecursos, (char)(_SEP_RECURSO_[0]) );	/*27/06/2007	GT	Modificacion para que lea un char y no un string*/
	int pos;
	
	if ( strlen(szRecursos) == 0 )
		return FALSE;
	
	strcpy( szEncrips, szRecursos );/*Hago esto porque el strtok afecta al char de entrada*/
	
	pAux = strtok( szEncrips, _SEP_RECURSO_ );
	
	
	while ( pAux )
	{
		if ( strcmp( pAux, _NO_SEP_ ) != 0 )
		{
			if ( !Rec_Buscar( pLista, pAux, nTope, &pos ) )
				return FALSE;			
		}
		
		pAux = strtok( NULL, _SEP_RECURSO_ );
		nContSep--;
	}
	
	if ( nContSep == -1 )	/*24/06/2007	GT	Cambio para que terminen los recursos sin ';'*/
		return TRUE;
	
	return FALSE;
}

/*******************************************************************/
int Rec_ConvertirVect( tDatosRecurso *pLista, const char* szRecursos, const int nTope, int *pInstancias ) 
{
	char szEncrips[256];
	char *pAux;
	int	 nContSep = ContarCharEnString( szRecursos, (char)(_SEP_RECURSO_[0]) ); 	/*27/06/2007	GT	Modificacion para que tome un char y no un string*/
	int pos;
	
	if ( strlen(szRecursos) == 0 )
		return FALSE;
	
	strcpy( szEncrips, szRecursos );/*Hago esto porque el strtok afecta al char de entrada*/
	
	pAux = strtok( szEncrips, _SEP_RECURSO_ );
	
	
	while ( pAux )
	{
		if ( strcmp( pAux, _NO_SEP_ ) != 0 )
		{
			if ( !Rec_Buscar( pLista, pAux, nTope, &pos ) )
				return FALSE;
			else
			{
				pInstancias[pos]++;	/*Se van sumando aquellos recursos que esta pidiendo en el orden correspondiente del vector*/
			}			
		}
		
		pAux = strtok( NULL, _SEP_RECURSO_ );
		nContSep--;
	}
	
	if ( nContSep == -1 )	/*24/06/2007	GT	Cambio para que terminen los recursos sin ';'*/
		return TRUE;
	
	return FALSE;	
}

/*******************************************************************/
void Rec_ObtenerVectorDisponibles( tDatosRecurso *pLista, const int nTope, int* pInstancias )
{
	int i;
	bzero(pInstancias,sizeof(pInstancias));

	for ( i = 0; i < nTope; i++ )
		pInstancias[i] = (pLista + i)->nAvailable;
	
}
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
