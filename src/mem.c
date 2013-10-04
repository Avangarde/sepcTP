/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mem.h"
#include <stdbool.h>

/** squelette du TP allocateur memoire */

void *zone_memoire = 0;

typedef struct element {
    struct element * suivant;
} element;

int SIZE[WBUDDY_MAX_INDEX];
int SUBBUDDY[WBUDDY_MAX_INDEX];
element * TZL[2*WBUDDY_MAX_INDEX];

int trouver_idx_size(int size) {
    int derniere_size = SIZE[0];
    int cmpt = 0;
    while (derniere_size <= size) {
        if (SIZE[cmpt] == size) {
            return cmpt;
        }
        derniere_size = SIZE[++cmpt];
    }

    perror("trouver_idx_size:");
    return -1;
}

bool verifier_adr_mem(void *ptr) {
    return ((ptr >= zone_memoire) && (ptr <= (zone_memoire + ALLOC_MEM_SIZE)));
}

void fusioner_buddys(int idx_cible, void *adr) {
    int idx_courante = WBUDDY_MAX_INDEX-1;

    void *min = zone_memoire;
    void *max = min + ALLOC_MEM_SIZE;

    void *adr_buddy;

    int buddysize;

    //Recherche dichotomique
    while (idx_courante > idx_cible - 3) {
        if (min == adr && idx_cible == idx_courante) {
            break;
        }
        if ((min + SIZE[SUBBUDDY[idx_courante]]) <= adr) {
            adr_buddy = min;
            buddysize = SIZE[SUBBUDDY[idx_courante]];
            min = min + SIZE[SUBBUDDY[idx_courante]];
            idx_courante--;
        } else {
            buddysize = SIZE[idx_courante - 1];
            max = min + SIZE[SUBBUDDY[idx_courante]];
            adr_buddy = max;
            idx_courante = SUBBUDDY[idx_courante];
        }
    }
    
    int idxbuddy = trouver_idx_size(buddysize);

    if (TZL[idxbuddy] != adr_buddy && TZL[idxbuddy] != NULL) {
        element * elm = TZL[idxbuddy];
        while (elm->suivant != NULL && elm->suivant != adr_buddy) {
            elm = elm->suivant;
        }
        if (elm->suivant == adr_buddy) {
            if (verifier_adr_mem(elm->suivant->suivant)){
                elm->suivant = elm->suivant->suivant;
            } else {
                elm->suivant = NULL;
            }
        } else {
            return;
        }
    } else if (TZL[idxbuddy] == adr_buddy) {
        TZL[idxbuddy] = NULL;
    } else {
        return;
    }
    
    if (TZL[idx_cible] != adr) {
        element * elm = TZL[idx_cible];
        while (elm->suivant != adr) {
            elm = elm->suivant;
        }
        if (verifier_adr_mem(elm->suivant->suivant)){
            elm->suivant = elm->suivant->suivant;
        } else {
            elm->suivant = NULL;
        }
    }else{
        if (verifier_adr_mem(TZL[idx_cible]->suivant)){
            TZL[idx_cible] = TZL[idx_cible]->suivant;
        } else {
            TZL[idx_cible] = NULL;
        }
    }
    
    int new_size = buddysize + SIZE[idx_cible];
    int new_idx = trouver_idx_size(new_size);
    
    void * new_adr;
    if (adr < adr_buddy){
        new_adr = adr;
    }else{
        new_adr = adr_buddy;
    }
    
    if (TZL[new_idx] != NULL) {
        element * elm = TZL[new_idx];
        while (elm->suivant != NULL) {
            elm = elm->suivant;
        }
        elm->suivant = new_adr;
    } else {
        TZL[new_idx] = new_adr;
    }

    if (new_idx < 39){
        fusioner_buddys(new_idx, new_adr);
    }
}

int mem_init() {
    if (!zone_memoire)
        zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
    if (zone_memoire == 0) {
        perror("mem_init:");
        return 1;
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
        SUBBUDDY[i] = trouver_idx_size(SIZE[i] - SIZE[i - 1]);
    }

    //Initialisation de TZL
    TZL[WBUDDY_MAX_INDEX - 1] = (element*) zone_memoire;
    TZL[WBUDDY_MAX_INDEX - 1]->suivant = NULL;
    return 0;
}

void * mem_alloc(unsigned long size) {
	if (size > 0 && size <= ALLOC_MEM_SIZE) {
		int i;
		for (i = WBUDDY_MAX_INDEX - 1; i >= 0; i--) {
			if (TZL[i] != NULL && SIZE[i] >= size) {
				if (SIZE[i] == size) {
					element * elem = TZL[i];
					if (elem->suivant !=NULL) {
						element * parent = TZL[i];
						while (elem->suivant != NULL && verifier_adr_mem(elem->suivant)) {
							parent = elem;
							elem = elem->suivant;
						}
						parent->suivant = NULL;
					} else {
						TZL[i] = NULL;
					}
					return (void*) elem;
				} else {
					//Faire les partitions
					element * temp = TZL[i];
					if (i < 37 && temp->suivant != NULL){
						element * parent = TZL[i];
						while (temp->suivant != NULL && verifier_adr_mem(temp->suivant)) {
							parent = temp;
							temp = temp->suivant;
						}
						parent->suivant = NULL;
					} else {
						TZL[i] = NULL;
					}
					if (TZL[SUBBUDDY[i]] != NULL) {
						element * elm = TZL[SUBBUDDY[i]];
						while (elm->suivant != NULL && verifier_adr_mem(elm->suivant)) {
							elm = elm->suivant;
						}
						elm->suivant = temp;
					} else {
						TZL[SUBBUDDY[i]] = temp;
					}
					if (TZL[i - 1] != NULL) {
						element * elm = TZL[i - 1];
						while (elm->suivant != NULL && verifier_adr_mem(elm->suivant)) {
							elm = elm->suivant;
						}
						elm->suivant = (element*) ((char*) temp + SIZE[SUBBUDDY[i]]);
					} else {
						TZL[i - 1] = (element*) ((char*) temp + SIZE[SUBBUDDY[i]]);
					}
					
				}
			}
		}
		if (i == -1) {
			perror("mem_alloc:");
			return 0;
		}
		return 0;
	} else {
	   perror("mem_alloc:");
	   return 0; 
    }
}

int mem_free(void *ptr, unsigned long size) {

    //la memoire a ete detruite
    if (zone_memoire == NULL) {
        perror("mem_free:");
        return -1;
    }

    // on ne peut pas liberer une zone situe hors de la zone permis
    if (!verifier_adr_mem(ptr)) {
        perror("mem_free:");
        return 1;
    }

    int idx = trouver_idx_size(size);
    if (TZL[idx] != NULL) {
        element * elm = TZL[idx];
        while (elm->suivant != NULL && verifier_adr_mem(elm->suivant)) {
            elm = elm->suivant;
        }
        elm->suivant = ptr;
    } else {
        TZL[idx] = ptr;
    }
    fusioner_buddys(idx, ptr);

    return 0;
}

int mem_destroy() {
    if (zone_memoire == NULL) {
        perror("mem_destroy:");
        return 1;
    }
    int i;
    for (i = 0; i < WBUDDY_MAX_INDEX; i++){
        if (TZL[i] != NULL){
	        TZL[i]->suivant = NULL;
            TZL[i] = NULL;
        }
    }
    free(zone_memoire);
    zone_memoire = 0;
    return 0;
}
