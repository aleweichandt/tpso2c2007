/*
 * estructuras.h
 *
 * Implementación de listas, pilas y colas adaptables a lo que venga.
 * Los algoritmos son (C) 1950 Jorge Durañona 
 *
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#ifndef	NULL
#define	NULL	(void *) 0
#endif

typedef struct
{
    void *sgte;		/* apunta a siguiente elemento */
    unsigned t;		/* cantidad de bytes de datos guardados en el buffer */
    void *datos;	/* apunta a un buffer con los datos */
} t_nodo;

t_nodo *nodo_sgte (t_nodo *);
void *nodo_datos (t_nodo *, unsigned *);
int nodo_liberar (t_nodo *);

int pila_push (t_nodo **, void *, unsigned);
void *pila_pop (t_nodo **, unsigned *);
int pila_vacia (t_nodo **);
int pila_inic (t_nodo **);

void* cola_agregar (t_nodo **, const void *, unsigned);
void *cola_quitar (t_nodo **, unsigned *);
int cola_vacia (t_nodo **);
int cola_inic (t_nodo **);
int cola_eliminar (t_nodo **, void *, int (*) (const void *, const void *));


void *lista_insertar (t_nodo **, const void *, unsigned, int (*) (const void *, const void *), int);
void *lista_reordenar (t_nodo **, int (*) (const void *, const void *));
int lista_inic (t_nodo **);
int lista_destruir (t_nodo **);
int lista_quitar (t_nodo **, void *, int (*) (const void *, const void *));
int lista_contar (t_nodo **);
t_nodo *lista_buscar (t_nodo **, const void *, int (*) (const void *, const void *));


#endif /*ESTRUCTURAS_H_*/
