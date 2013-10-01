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

typedef struct element {
    struct element *suivant;
} Element;

int SIZE[WBUDDY_MAX_INDEX];
int SUBBUDDY[WBUDDY_MAX_INDEX];
Element * TZL[WBUDDY_MAX_INDEX];

int find_index(int a[], int num_elements, int value) {
    int i;
    for (i = 0; i < num_elements; i++) {
        if (a[i] == value) {
            return (i); /* it was found */
        }
    }
    return (-1); /* if it was not found */
}

int mem_init() {
    if (!zone_memoire)
        zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
    if (zone_memoire == 0) {
        perror("mem_init:");
        return -1;
    }

    //Initialisation de SIZE
    int i, j = 0;
    SIZE[0] = 1;
    for (i = 1; i < WBUDDY_MAX_INDEX / 2; i++) {
        SIZE[i + j] = 1 << i;
        SIZE[i + j + 1] = 3 * 1 << (i - 1);
        j++;
    }
    SIZE[WBUDDY_MAX_INDEX - 1] = 1 << i;

    //Initialisation de SUBBUDDY
    SUBBUDDY[0] = 0;
    for (i = 1; i < WBUDDY_MAX_INDEX; i++) {
        SUBBUDDY[i] = find_index(SIZE, WBUDDY_MAX_INDEX, (SIZE[i] - SIZE[i - 1]));
    }

    //Initialisation de TZL
    TZL[WBUDDY_MAX_INDEX - 1] = (Element*)zone_memoire;

    return 0;
}

/*allocation d’une zone mémoire initialement libre de taille tailleZone. La fonction retournera le pointeur vers cette zone mémoire.

Le retour sera (void *)0 en cas d’erreur ou s’il n’existe plus d’emplacement libre de taille
tailleZone.
 */
void * mem_alloc(unsigned long size) {
    /*  ecrire votre code ici */
    int i;
    for (i = WBUDDY_MAX_INDEX-1; i >= 0; i--){
		if (TZL[i] != NULL && SIZE[i] >= size) {
			void *adr;
			if (SIZE[i] == size) {
				adr = TZL[i];
				if (TZL[i]->suivant != NULL){
					TZL[i] = TZL[i]->suivant;
				} else {
					TZL[i] = NULL;
				}
			} else {
				//Faire les partitions
				if (i % 2 == 1){ //2^k
					
				} else { //3*2^k
					
				}
			}
			return adr;
	    }
    }
    if (i == -1) {
        perror("mem_alloc:");
        return NULL;
    }
    return 0; 
}

/*
libère la zone commençant
à l’adresse zone de taille tailleZone (valeur renvoyée par mem_alloc). Un retour d’erreur
sera retourné en cas de problème, sinon 0 sera retourné si tout s’est bien passé.
 */
int mem_free(void *ptr, unsigned long size) {
    /* ecrire votre code ici */

    //la memoire a ete detruite
    if (zone_memoire == NULL) {
        perror("mem_free:");
        return 1;
    }

    // on ne peut pas liberer une zone situe hors de la zone permis

    /*if ((ptr < zone_memoire) || (ptr > zone_memoire + HEAP_SIZE)) {
        perror("mem_free:");
        return 1;
    }*/




    return 0;
}

/*libère toutes les structures et la zone de mémoire utilisées.*/

int mem_destroy() {
    /* ecrire votre code ici */

    if (zone_memoire == NULL) {
        perror("mem_destroy:");
        return 1;
    }

    free(zone_memoire);
    zone_memoire = 0;
    return 0;
}

int main(void) {
    mem_init();
    printf("%p\n", zone_memoire);
    int i;
    for (i = 0; i < WBUDDY_MAX_INDEX; i++) {
        printf("%d ", i);
        printf("%d \t", SIZE[i]);
        printf("%d \t", SUBBUDDY[i]);
        printf("%p \n", TZL[i]);
    }
    printf("%p\n", mem_alloc(1048576));
    for (i = 0; i < WBUDDY_MAX_INDEX; i++) {
        printf("%d ", i);
        printf("%d \t", SIZE[i]);
        printf("%d \t", SUBBUDDY[i]);
        printf("%p \n", TZL[i]);
    }
    return 0;
}
