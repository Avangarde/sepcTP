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

int trouver_idx_size(int size) {
    int derniere_size = SIZE[0];
    int cmpt = 0;
    while (derniere_size <= size) {
        if (SIZE[cmpt] == size) {
            //je l'ai trouve
            return cmpt;
        }
        derniere_size = SIZE[++cmpt];
    }

    perror("trouver_idx_size:");
    return -1;
}

void fusioner_buddys(int idx_cible, void *adr) {
    //est que je suis 2^n ou 3x2^n

    //trouver mon buddy

    int idx_courante = WBUDDY_MAX_INDEX-1;

    void *min = zone_memoire;
    void *max = min + ALLOC_MEM_SIZE;

    //valeur a retorner
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

    //Necesito conocer simi buddy esta ocupado, si no lo esta he de borrarlo de una vez
    if (TZL[idxbuddy] != adr_buddy && TZL[idxbuddy] != NULL) {
        Element * elm = TZL[idxbuddy];
        while (elm->suivant != NULL && elm->suivant != adr_buddy) {
            elm = elm->suivant;
        }
        if (elm->suivant == adr_buddy) {
            elm->suivant = elm->suivant->suivant;
        } else {
            //Problema... no lo he encontrado, entonces no esta disponible
            return;
        }
    } else if (TZL[idxbuddy] == adr_buddy) {
        TZL[idxbuddy] = NULL;
    } else {
        return;
    }
    

    //Dado que la ejecucion continuo sin retornar, es seguro borrarme de la lista y fusionar.
    //Deberia estar en la lista, luego no se hacen comprobaciones en esta parte

    if (TZL[idx_cible] != adr) {
        Element * elm = TZL[idx_cible];
        while (elm->suivant != adr) {
            elm = elm->suivant;
        }
        elm->suivant = elm->suivant->suivant;
    }else{
        TZL[idx_cible] = TZL[idx_cible]->suivant;
    }
    
    //Ahora creamos un slot nuevo con el bloque largo, pero necesito saber donde lo guardare.
    int new_size = buddysize + SIZE[idx_cible];
    int new_idx = trouver_idx_size(new_size);
    
    void * new_adr;
    if (adr < adr_buddy){
        new_adr = adr;
    }else{
        new_adr = adr_buddy;
    }
    
    if (TZL[new_idx] != NULL) {
        Element * elm = TZL[new_idx];
        while (elm->suivant != NULL) {
            elm = elm->suivant;
        }
        elm->suivant = new_adr;
    } else {
        TZL[new_idx] = new_adr;
    }

    //Llamamos recursivamente al metodo para intentar continuar

    fusioner_buddys(new_idx, new_adr);


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
    TZL[WBUDDY_MAX_INDEX - 1] = (Element*) zone_memoire;

    return 0;
}

void * mem_alloc(unsigned long size) {
    int i;
    for (i = WBUDDY_MAX_INDEX - 1; i >= 0; i--) {
        if (TZL[i] != NULL && SIZE[i] >= size) {
            if (SIZE[i] == size) {
                Element * elem = TZL[i];
                Element * parent = TZL[i];
                    while (elem->suivant != NULL) {
                        parent = elem;
                        elem = elem->suivant;
                    }
                    parent->suivant = NULL;
                return (void*) elem;
            } else {
                //Faire les partitions
                if (TZL[SUBBUDDY[i]] != NULL) {
                    Element * elm = TZL[SUBBUDDY[i]];
                    while (elm->suivant != NULL) {
                        elm = elm->suivant;
                    }
                    elm->suivant = TZL[i];
                } else {
                    TZL[SUBBUDDY[i]] = TZL[i];
                }
                if (TZL[i - 1] != NULL) {
                    Element * elm = TZL[i - 1];
                    while (elm->suivant != NULL) {
                        elm = elm->suivant;
                    }
                    elm->suivant = (Element*) ((char*) TZL[i] + SIZE[SUBBUDDY[i]]);
                } else {
                    TZL[i - 1] = (Element*) ((char*) TZL[i] + SIZE[SUBBUDDY[i]]);
                }
                if (TZL[i]->suivant != NULL) {
                    TZL[i] = TZL[i]->suivant;
                } else {
                    TZL[i] = NULL;
                }
            }
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

    if ((ptr < zone_memoire) || (ptr > zone_memoire + ALLOC_MEM_SIZE)) {
        perror("mem_free:");
        return 1;
    }

    // Je connais mon size, alors je vais trouver ma place dans tzl, apres je serai ajouté a la liste et deviendrai free et allocable
    int idx = trouver_idx_size(size);
    //Laisse un ptr sur l'adresse que je dois ajouter
    if (TZL[idx] != NULL) {
        Element * elm = TZL[idx];
        while (elm->suivant != NULL) {
            elm = elm->suivant;
        }
        elm->suivant = ptr;
    } else {
        TZL[idx] = ptr;
    }

    //On essai de faire la fusion entre les buddys... le petit à gauche et le grande à droit... fait dans autre methode.

    fusioner_buddys(idx, ptr);

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
    void * mem = mem_alloc(262144);
    printf("%p\n", mem);
    int i;
    for (i = 0; i < WBUDDY_MAX_INDEX; i++) {
        printf("%d ", i);
        printf("%d \t", SIZE[i]);
        printf("%d \t", SUBBUDDY[i]);
        printf("%p", TZL[i]);
        Element * elm = TZL[i];
        while (elm != NULL && elm->suivant != NULL) {
            if (elm->suivant != NULL) {
                printf("\t %p", elm->suivant);
                elm = elm->suivant;
            }
        }
        printf("\n");
    }
    
    mem_free(mem, 262144);
    

    return 0;
}
