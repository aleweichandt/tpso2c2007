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
int Rec_IncrementarInst( tDatosRecurso *pE, const int bConLiberacion )
/*15/06/2007	GT	Se incrementan por su disponibilidad, nunca por su total */
{
	int i;
	
	if( bConLiberacion && pE->nAvailable < pE->nInstancias )		/*15/06/2007	GT	para corregir defasaje */
		++(pE->nAvailable);
	++(pE->nSemaforo);
		
	return pE->nAvailable;
}

/***********************************************************************************/
int Rec_DecrementarInst( tDatosRecurso *pE, const int bConAsignacion )
{
	int i;
	int bResultado= FALSE;
			
	if( bConAsignacion && pE->nAvailable > 0 )		/*15/06/2007	GT	para corregir defasaje */
	{
		--(pE->nAvailable);
		bResultado = TRUE;
	}else if( !bConAsignacion )		/*13/11/2007	GT	porque en la asignacion se supone el semaforo ya descontado*/
	{
		--(pE->nSemaforo);
	}
	
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

/*******************************************************************/
int Rec_AgregarBloqueado(tDatosRecurso* recurso, long ppcbid){
	int pos = 0;
	while( pos < MAX_LISTA_BLOQ && recurso->ListaBloqueados[pos] )
		pos++;
	if( pos < MAX_LISTA_BLOQ ){
		recurso->ListaBloqueados[pos] = ppcbid;
		return OK;
	}
	return ERROR;	/*no se pudo agregar, capacidad de la lista excedido*/
}

/*******************************************************************/
long Rec_ObtenerBloqueado(tDatosRecurso* recurso, int pos){
	return (pos<MAX_LISTA_BLOQ)? recurso->ListaBloqueados[pos]:ERROR;
}

/*******************************************************************/
long Rec_QuitarBloqueado(tDatosRecurso* recurso){
	int pos = 1;
	long returnVal = recurso->ListaBloqueados[0];
	while( pos < MAX_LISTA_BLOQ ){
		recurso->ListaBloqueados[pos-1] = recurso->ListaBloqueados[pos];
		pos++;
	}
	return returnVal;
}

long Rec_EliminarPidDeBloqueados(tDatosRecurso* recurso, long pid){
	int pos = 0;
	long ppcbid = 0;
	while( (ppcbid = Rec_ObtenerBloqueado(recurso, pos)) > 0){
		
		if( ppcbid == pid ){
			/*Encontre el pid y debo eliminarlo de la lista*/
			recurso->nSemaforo++;
			pos++;
			while( pos < MAX_LISTA_BLOQ ){
				recurso->ListaBloqueados[pos-1] = recurso->ListaBloqueados[pos];
				pos++;
			}
			return;
		}
		
		pos++;
	}
}

/*******************************************************************/
int Rec_SinBloqueados(tDatosRecurso* recurso){
	return recurso->ListaBloqueados[0] == (long)NULL;
}
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
