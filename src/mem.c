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
typedef struct element {
	struct element *suivant;
} Element;

int SIZE[WBUDDY_MAX_INDEX];
int SUBBUDDY[WBUDDY_MAX_INDEX];
Element TZL[WBUDDY_MAX_INDEX];

/*alloue la zone de mémoire (c’est le seul malloc autorisé dans votre code)et réalise l’initialisation des structures de données utilisées par le gestionnaire d’allocation de la mémoire. L’adresse de la zone sera conservée dans la variable globale zone_memoire.
La fonction retournera un code d’erreur indiquant si l’initialisation s’est bien passée ou non. Elle renvoie 0 si tout s’est bien passé. */
int mem_init() {
  /*if (! zone_memoire)
    zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
  if (zone_memoire == 0) {
    perror("mem_init:");
    return -1;
  }*/

  /* ecrire votre code ici */
  int i, j = 0;
  SIZE[0] = 1;
  for (i = 1; i < WBUDDY_MAX_INDEX/2; i++){
	  SIZE[i+j] = 1<<i;
	  SIZE[i+j+1] = 3 * 1<<(i-1);
	  j++;
  }
  SIZE[WBUDDY_MAX_INDEX-1] = 1<<i;

  return 0;
}

/*allocation d’une zone mémoire initialement libre de taille tailleZone. La fonction retournera le pointeur vers cette zone mémoire.

Le retour sera (void *)0 en cas d’erreur ou s’il n’existe plus d’emplacement libre de taille
tailleZone.
*/
void * mem_alloc(unsigned long size) {
  /*  ecrire votre code ici */
  return 0;  
}


/*
libère la zone commençant
à l’adresse zone de taille tailleZone (valeur renvoyée par mem_alloc). Un retour d’erreur
sera retourné en cas de problème, sinon 0 sera retourné si tout s’est bien passé.
*/
int mem_free(void *ptr, unsigned long size) {
  /* ecrire votre code ici */
  return 0;
}

/*libère toutes les structures et la zone de mémoire utilisées.*/

int mem_destroy() {
  /* ecrire votre code ici */

  free(zone_memoire);
  zone_memoire = 0;
  return 0;
}

int main (void) {
	mem_init();
	int i;
	for (i = 0; i < WBUDDY_MAX_INDEX; i++){
	  printf("%d ", i);
	  printf("%d \n", SIZE[i]);
	}
        
    return 0;
}
