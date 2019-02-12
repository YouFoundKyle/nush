#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "vector.h"

/** Got From Class Notes when Professor was going over Calculator in CMD line */
vector*
make_vector()
{
    vector* vec = malloc(sizeof(vector));
    vec->size = 0;
    vec->cap  = 4;
    vec->data = malloc(4 * sizeof(char*));
    memset(vec->data, 0, 4 * sizeof(char*));
    return vec;
}

void
free_vector(vector* vec)
{
    for (int ii = 0; ii < vec->size; ++ii) {
        if (vec->data[ii] != 0) {
            free(vec->data[ii]);
        }
    }
    free(vec->data);
    free(vec);
}

char*
vector_get(vector* vec, int ii)
{
    assert(ii >= 0 && ii < vec->size);
    return vec->data[ii];
}

void
vector_put(vector* vec, int ii, char* tok)
{
    assert(ii >= 0 && ii < vec->size);
    vec->data[ii] = strdup(tok);

}


void 
vector_push(vector* vec, char* tok)
{
    int ii = vec->size;

    if (ii >= vec->cap) {
        vec->cap *= 2;
        vec->data = (char**) realloc(vec->data, vec->cap * sizeof(char*));
    }

    vec->size = ii + 1;
    vector_put(vec, ii, tok);
}
