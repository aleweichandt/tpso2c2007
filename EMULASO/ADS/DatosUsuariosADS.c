/*
 *<<---------------- KERNEL PANIC ATTACK ----------------------------->>
 * Archivo: DatosTransfADE.c
 * 
 * Historia
 *	16-05-2007	GT	Creacion
 *  18/06/2007	GT	Implementacion de funciones
 ------------------------------------------------------------------ 
 */

#include "DatosUsuariosADS.h"

/*********************************************************/
void UsuariosADS_IniciarLista (tListaUsuariosADS lista)
{
	lista_inic(&lista);
}

/*********************************************************/
int UsuariosADS_AgregarUsr (tListaUsuariosADS *lista, const int idConn, const unsigned char *mshellAsoc, const char *userName, unsigned char estado)
{
	int pos;
	tUsuarioADS* usrADS = malloc(sizeof(tUsuarioADS));
	usrADS->IdConeccion = idConn;
	usrADS->key = -1;
	memcpy(usrADS->MshellAsociado, mshellAsoc, 4);
	strcpy(usrADS->Usuario, userName);
	usrADS->Estado = estado;
	lista_insertar(lista, usrADS, sizeof(tUsuarioADS), &compararUsuariosADS, _SIN_REPET_ );
	UsuariosADS_BuscarUsr(lista,idConn,&pos);
	return pos;
}

/*********************************************************/
void	UsuariosADS_EliminarUsr(tListaUsuariosADS *lista, const int idConn)
{
	int pos;
	tUsuarioADS usrBuscado;
	usrBuscado.IdConeccion = idConn;
	lista_quitar(lista,&usrBuscado,compararUsuariosADS);
	
}

/*********************************************************/
void TransfAde_EliminarTodas(tListaUsuariosADS *lista)
{
	lista_destruir(lista);
}

/*********************************************************/
tUsuarioADS*	UsuariosADS_BuscarUsr	(tListaUsuariosADS *lista, const int idConn, int *pos)
{
	unsigned int cantBytes;
	tListaUsuariosADS listaAux = *lista;
	tUsuarioADS usrBuscado ;
	tUsuarioADS *usrADS = NULL;
	
	usrBuscado.IdConeccion = idConn;
	
	*pos = 0;
	while(listaAux)
	{
		usrADS = (tUsuarioADS*)nodo_datos(listaAux,&cantBytes);
		if(compararUsuariosADS ( (void*)&usrBuscado, (void*)usrADS ) == 0)
			return usrADS; 
		listaAux = listaAux->sgte;
		++*pos;
	}
	return NULL;
}

/*********************************************************/
tUsuarioADS*	UsuariosADS_Datos	(tListaUsuariosADS lista)
{
	unsigned int alpe;
	return nodo_datos ( lista, &alpe );
}

/*********************************************************/
tListaUsuariosADS	UsuariosADS_Siguiente	(tListaUsuariosADS lista)
{
	return nodo_sgte( lista );
}

/*********************************************************/
int compararUsuariosADS( const void *usr1, const void *usr2 )
{
	tUsuarioADS *usrADS1 = (tUsuarioADS*) usr1,
			    *usrADS2 = (tUsuarioADS*) usr2;
			   
	return (int)(usrADS1->IdConeccion - usrADS2->IdConeccion); 
}
/**********************************************************/
int ImprimirUsuariosADS(tListaUsuariosADS *lista, char* szPathArchivoEstado)
{
	unsigned int cantBytes;
	tListaUsuariosADS listaAux = *lista;
	tUsuarioADS *usrADS = NULL;
	FILE *fp;
	
	if( ( fp = fopen( szPathArchivoEstado, "a+" ) ) == NULL )
	{
		printf("Error al abrir/crear el archivo de Estado\n");
		return ERROR;
	}
	while(listaAux)
	{
		usrADS = (tUsuarioADS*)nodo_datos(listaAux,&cantBytes);
		fprintf(fp, "IdSesion = %d   Usuario = %s,   Mshell = %s", 
				usrADS->IdConeccion, usrADS->Usuario, usrADS->MshellAsociado);
		listaAux = listaAux->sgte;
	}
	close(fp);
	return OK;
}
