#ifndef vector_H
#define vector_H

/** Got From Class Notes when Professor was going over Calculator in CMD line */

typedef struct vector {
    int size;
    int cap;
    char** data;
} vector;

vector* make_vector();
void  free_vector(vector* vec);

char* vector_get(vector* vec, int ii);
void  vector_put(vector* vec, int ii, char* tok);

void vector_push(vector* vec, char* tok);


#endif
