/*
 * estructuras.c
 *
 * Implementación de listas, pilas y colas adaptables a lo que venga.
 * Los algoritmos son (C) 1950 Jorge Durañona 
 *
 */

#include <string.h>
#include <stdlib.h>
#include "estructuras.h"

t_nodo *nodo_sgte (t_nodo *nodo)
{
	return (nodo->sgte);
}

void *nodo_datos (t_nodo *nodo, unsigned *t)
{
	if (t != NULL)
		*t = nodo->t;
	return (nodo->datos); /* los datos están en una ubicación segura */
}

int nodo_liberar (t_nodo *nodo)
{
	free (nodo->datos);	/* libero el buffer de datos primero */
	free (nodo);	/* libero el nodo */
	return (1);
}

int pila_push (t_nodo **p, void *datos, unsigned t)
{
	/* pongo datos en el tope de la pila y actualizo el puntero de pila */
	/* el primer parametro es la direccion del puntero de pila */
	/* el 2do son los datos, y el 3ro es la cantidad de bytes que componen los datos */
	t_nodo *nuevonodo = (t_nodo *) malloc (sizeof (t_nodo));	/* reservo espacio para el nuevo nodo */

	if (nuevonodo == NULL)
		return (-1);
	nuevonodo->sgte = *p;	/* dirección del nodo anterior */
	nuevonodo->t = t;
	nuevonodo->datos = (void *) malloc (t);	/* reservo espacio para los datos */
	memcpy (nuevonodo->datos, datos, t);	/* copio datos */
	*p = nuevonodo;	/* actualizo el puntero de pila */
	return (1);
}

void *pila_pop (t_nodo **p, unsigned *t)
{
	/* devuelve los datos que estan en el tope de la pila */
	/* y actualiza el puntero de pila */
	/* el primer parametro es la direccion del puntero de pila */
	/* el segundo parametro es la direccion de un int en donde */
	/* opcionalmente se guarda la cantidad de bytes leidos */
	t_nodo *sgte; void *datos;

	if (*p == NULL)
		return (NULL); 	/* si la pila esta vacía devuelvo NULL */
	if (t != NULL)
		*t = (*p)->t; 	/* tamaño de los datos del nodo (parámetro de salida opcional) */
	datos = (*p)->datos; 	/* los datos están en una posición segura */
	sgte = (*p)->sgte;
	free (*p);	/* libero la memoria del nodo */
	*p = sgte;	/* actualizo el puntero de pila */
	return (datos);
}

int pila_vacia (t_nodo **p)
{
	return (*p == NULL);
}
 
int pila_inic (t_nodo **p)
{
	 *p = NULL;
	 return (1);
}

void* cola_agregar (t_nodo *c[2], const void *datos, unsigned t)	/* en un array paso "colaFte" [0]  y "colaFin" [1] */
{
	/* agrego datos al fin de la cola */
	/* el primer parametro es el array con los punteros de la cola */
	/* el 2do es el puntero a los datos, mientras que el tercero es */
	/* la cantidad de bytes de los datos */
	t_nodo *nuevonodo = (t_nodo *) malloc (sizeof (t_nodo));	/* reservo espacio para el nuevo nodo */

	if (nuevonodo == NULL)
		return (NULL);
	nuevonodo->sgte = NULL;
	nuevonodo->t = t;
	nuevonodo->datos = (void *) malloc (t);	/* reservo espacio para datos */
	memcpy (nuevonodo->datos, datos, t);	/* copio datos */
	if (c[0] == NULL)	/* la cola está vacía */
		c[0] = c[1] = nuevonodo;
	else
	{
		c[0]->sgte = nuevonodo;	/* linkeo */
		c[0] = nuevonodo;	/* asigno el nuevo valor a frente */
	}
	return nuevonodo->datos;
}

void *cola_quitar (t_nodo *c[2], unsigned *t)
{
	/* devuelvo los datos del primer nodo en el "frente de la cola" */
	/* y actualizo los punteros de la cola */
	/* el primer parametro es el array con los dos punteros de la cola */
	/* t es la direccion de un int donde se guarda opcionalmente la cantidad */
	/* de bytes en los datos que se devuelven. Si no se usa, pasar NULL */
	
	t_nodo *sgte; void *datos;

	if (c[1] == NULL)
		return (NULL);	/* si la cola esta vacía devuelvo NULL */
	if (t != NULL)
		*t = c[1]->t;	/* tamaño de los datos del nodo (parámetro de salida opcional) */
	datos = c[1]->datos;	/* los datos están en una ubicación segura */
	sgte = c[1]->sgte;
	free (c[1]);	/* libero la memoria del nodo */
	c[1] = sgte;	/* actualizo el puntero de fin de cola */
	if (c[1] == NULL)
		c[0] = NULL;	/* si la pila esta vacía, le doy "consistencia" */
	return (datos);
}

int cola_vacia (t_nodo *c[2])
{
	return (c[0] == NULL);
}

int cola_inic (t_nodo *c[2])
{
	/* inicializo los dos punteros de una cola */
	c[0] = c[1] = NULL;
	return (1);
}

int cola_eliminar (t_nodo *c[2], void *param, int (*comp) (const void *, const void *))
{
	t_nodo *colanueva[2];
	
	cola_inic (colanueva);
	while (!cola_vacia (c))
	{
		unsigned t;
		void *datos;
		
		datos = cola_quitar (c, &t);
		if (comp (datos, param) != 0)
			cola_agregar (colanueva, datos, t);
		else
			free (datos);
	}
	c[0] = colanueva[0];
	c[1] = colanueva[1];
	return (1);
}

void *lista_insertar (t_nodo **l, const void *datos, unsigned t, int (*comp) (const void *, const void *), int sinrep)
{
	/* inserto datos en la lista con algun criterio de ordenamiento */
	/* pasado a traves de comp()... */
	/* el primer parametro es un puntero al puntero de la lista, el segundo */
	/* apunta a los datos que se quieren insertar, el tercero es la cantidad */
	/* de bytes que se insertan, el cuarto apunta a la funcion comparadora */
	/* y sinrep != 0 para listas sin repeticion. */
	/* en caso de que sea una lista sin repeticion, devuelve NULL */
	/* sacando esa excepcion, siempre se devuelve un puntero a los datos del nuevo nodo creado */ 
	
	/* sobre comp() */
	/* parecida a strcmp() ... */
	/* si a < b => < 0 */
	/* si a == b => == 0 */
	/* si a > b => > 0 */
	t_nodo *p, *q = NULL, *nuevonodo = (t_nodo *) malloc (sizeof (t_nodo)); /* reservo espacio para el nuevo nodo */

	if (nuevonodo == NULL)
		return (NULL);
	nuevonodo->t = t;
	nuevonodo->datos = (void *) malloc (t);	/* reservo espacio para el buffer de datos */
	memcpy (nuevonodo->datos, datos, t);	/* copio los datos */
	if (*l == NULL || comp (datos, (*l)->datos) < 0) /* la lista está vacía o el elemento debe ubicarse primero */
	{
		nuevonodo->sgte = *l;
		*l = nuevonodo;
		return (nuevonodo->datos);
	}
	p = *l; 
	/* recorro la estructura secuencialmente para ver dónde pongo los datos */
	while (p != NULL && comp (datos, p->datos) >= 0)
	{
		q = p;
		p = p->sgte;
	}
	/* para listas sin repeticion */
	if (sinrep && q != NULL && comp (datos, q->datos) == 0)
	{
		nodo_liberar (nuevonodo);	/* no lo voy a usar */
		return NULL;
	}
	nuevonodo->sgte = q->sgte;	/* link */
	q->sgte = nuevonodo;
	return (nuevonodo->datos);
}

void *lista_reordenar (t_nodo **l, int (*comp) (const void *, const void *))
{
	/* recorro la lista vieja haciendo lista_insertar() con la nueva funcion comparadora */
	/* la lista vieja la vuelo y sobreescribo el valor del puntero */
	/* el primer parametro es el puntero al puntero de lista y el 2do la */
	/* la nueva funcion comparadora */
	t_nodo *listanueva = NULL, *q, *p = *l;

	while (p != NULL)
	{
		lista_insertar (&listanueva, p->datos, p->t, comp, 0);
		q = p;
		p = nodo_sgte (p);
		nodo_liberar (q);
	}
	return (*l = listanueva);
}

int lista_inic (t_nodo **l)
{
	/* inicializo la lista, tiene que invocarse siempre salvo que el puntero */
	/* sea una variable global */
	*l = NULL;
	return (1);
}

int lista_destruir (t_nodo **l)
{
	/* recorro una lista destruyendo su contenido y pongo el puntero en NULL */
	t_nodo *p, *q;

	p = *l;
	while (p != NULL)
	{
		q = p;
		p = p->sgte;
		nodo_liberar (q);
	}
	*l = NULL;
	return (1);
}

t_nodo *lista_buscar (t_nodo **l, const void *param, int (*comp) (const void *, const void *))
{
	t_nodo *p = *l;
	
	while (p != NULL)
	{
		if (comp (p->datos, param) == 0)
			return (p);
		p = nodo_sgte (p);
	}
	return (NULL);
}

int lista_quitar (t_nodo **l, void *param, int (*comp) (const void *, const void *))
{
	/* elimino el primer nodo para el cual comp() es 0 */
	/* el primer parametro es la direccion del puntero de lista */
	/* el 2do parametro es info extra que se pueda llegar a necesitar para la */
	/* funcion comparadora y el 3ero es la funcion comparadora */

	/* sobre comp() */
	/* el primer parametro de comp() es el buffer con los datos del nodo */
	/* el segundo parametro es la info que el usuario considere necesaria */
	/* por ejemplo, si se desea eliminar todos aquellos elementos iguales a un valor */
	/* pero ese valor no puede ser "hardcodeado", se usa el parametro extra */
	/* si no lo usas manda NULL */

	t_nodo *p = *l, *q = NULL, *r;

	while (p != NULL)
	{
		if (comp (p->datos, param) == 0) /* hay que volarlo */
		{
			if (q != NULL)
				q->sgte = p->sgte;
			else /* tengo que borrar el primer elemento de la lista */
				*l = nodo_sgte (p);
			r = p;
			p = nodo_sgte (p);
			nodo_liberar (r);   
			return (1);
		}
		else
		{
			q = p;
			p = nodo_sgte(p);
		}
	}
	return (0);
}

int lista_contar (t_nodo **l)
{
	/* devuelvo la cantidad de elementos en una lista */
	t_nodo *p;
	int i = 0;

	p = *l;
	while (p != NULL)
	{
		i++;
		p = nodo_sgte(p);
	}
	return (i);
}
