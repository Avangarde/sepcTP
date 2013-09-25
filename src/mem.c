/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mem.h"

/** squelette du TP allocateur memoire */

void *zone_memoire = 0;

/* ecrire votre code ici */


/*alloue la zone de mémoire (c’est le seul malloc autorisé dans votre code)et réalise l’initialisation des structures de données utilisées par le gestionnaire d’allocation de la mémoire. L’adresse de la zone sera conservée dans la variable globale zone_memoire.
La fonction retournera un code d’erreur indiquant si l’initialisation s’est bien passée ou non. Elle renvoie 0 si tout s’est bien passé. */
int 
mem_init()
{
  if (! zone_memoire)
    zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
  if (zone_memoire == 0)
    {
      perror("mem_init:");
      return -1;
    }

  /* ecrire votre code ici */

  return 0;
}

/*allocation d’une zone mémoire initialement libre de taille tailleZone. La fonction retournera le pointeur vers cette zone mémoire.

Le retour sera (void *)0 en cas d’erreur ou s’il n’existe plus d’emplacement libre de taille
tailleZone.
*/
void *
mem_alloc(unsigned long size)
{
  /*  ecrire votre code ici */
  return 0;  
}


/*
libère la zone commençant
à l’adresse zone de taille tailleZone (valeur renvoyée par mem_alloc). Un retour d’erreur
sera retourné en cas de problème, sinon 0 sera retourné si tout s’est bien passé.
*/
int 
mem_free(void *ptr, unsigned long size)
{
  /* ecrire votre code ici */
  return 0;
}

/*libère toutes les structures et la zone de mémoire utilisées.*/

int
mem_destroy()
{
  /* ecrire votre code ici */

  free(zone_memoire);
  zone_memoire = 0;
  return 0;
}

