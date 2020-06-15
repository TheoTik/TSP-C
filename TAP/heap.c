#include "heap.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

heap heap_create(int n, int (*f)(const void *, const void *)) {
    heap h = (heap) malloc(sizeof(*h));
    h->nmax = n;
    h-> array = malloc((n+1)*sizeof(void*));
    h-> f = f;
    h-> n = 0;
    return h;
}

void heap_destroy(heap h) {
    if (h != NULL){
        free(h->array);
        free(h);
    }
}

bool heap_empty(heap h) {
    return (h-> n == 0);
}

void swap(void* tab[], int i, int j){
    void* temp =  tab[i];
    tab[i]= tab [j];
    tab[j] = temp;
}

bool heap_add(heap h, void *object) {
    if (h == NULL)
        return true;
    if(h ->n == h->nmax)
        return true;
    h->n = h-> n +1;
    h-> array[h->n] = object;
    bool stop = false;
    int i = h->n;
    while(!stop){
        if (i == 1)
            stop = true;
        else
            if(h->f(h->array[i/2], h ->array[i]) > 0 ){
                swap(h->array,i,i/2);
                i = i/2;
            }
            else
                stop = true;
    }
    return false;
}

void *heap_top(heap h) {
    if(h== NULL)
        return NULL;
    if(h->n == 0)
        return NULL;
    return h->array[1];
}

void *heap_pop(heap h) {
    if(h == NULL)
        return NULL;
    if(h->n == 0)
        return NULL;
    void* o = h->array[1];
    h->array[1] = h->array[h->n];
    h->n = h->n -1;
    int i = 1;
    bool stop = false;
    while(!stop){
        if((i*2) > h->n){
            //pas de fils gauche
            stop = true;
        }
        else
        {
            if((i*2+1) <= h->n){
                //fils droit et gauche existent

                int iFilsMin;
                if(h->f(h->array[i*2],h->array[i*2+1]) > 0){
                    iFilsMin = i*2 + 1;
                }
                else{
                    iFilsMin = i*2;
                }
                if(h->f(h->array[i],h->array[iFilsMin]) > 0){
                    swap(h->array,i,iFilsMin);
                    i = iFilsMin;
                }
                else{
                    stop = true;
                }
            }
            else{
                //fils gauche mais pas de fils droit
                if(h->f(h->array[i],h->array[i*2]) > 0){
                    //fils gauche plus petit
                    swap(h->array, i, i*2);
                    stop = true;
                }
                else{
                    stop = true;
                }
            }
        }
    }
    return o;
}
