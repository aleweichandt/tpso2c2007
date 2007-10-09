#ifndef DATOSUSUARIOS_H_
#define DATOSUSUARIOS_H_
#include "../incGeneral/incGeneral.h"
#include "../incGeneral/Estructuras/estructuras.h"

typedef struct 
{
	int 			IdConeccion;
	unsigned char	MshellAsociado[4];
	char			Usuario[LEN_USERNAME];
	int				key;
	unsigned char	Estado;
} tUsuarioADS;

typedef enum
{
	ESPERANDO_PASS,
	CONECTADO
}tEstadoUsrADS;

#define tListaUsuariosADS t_nodo*

void				UsuariosADS_IniciarLista 	(tListaUsuariosADS lista);
int 				UsuariosADS_AgregarUsr		(tListaUsuariosADS *lista, const int idConn, const unsigned char *mshellAsoc, const char *userName, unsigned char estado);
void				UsuariosADS_EliminarUsr		(tListaUsuariosADS *lista, const int idConn);
void				UsuariosADS_EliminarTodas	(tListaUsuariosADS *lista);
tUsuarioADS*		UsuariosADS_BuscarUsr		(tListaUsuariosADS *lista, const int idConn, int *pos);
tUsuarioADS*		UsuariosADS_Datos			(tListaUsuariosADS lista);
tListaUsuariosADS	UsuariosADS_Siguiente		(tListaUsuariosADS lista);

int				compararUsuariosADS		( const void *usr1, const void *usr2 );
int 			ImprimirUsuariosADS		(tListaUsuariosADS *lista, char* pathArchivoEstado);
#endif /*DATOSUSUARIOS_H_*/
