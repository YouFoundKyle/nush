#ifndef tokens_H
#define tokens_H


vector* make_vector();
void  free_vector(vector* vec);

char* vector_get(vector* vec, int ii);
void  vector_put(vector* vec, int ii, char* tok);

int is_long_operator(const char* token);

char* read_operator(const char* text, int ii);

char* read_token(const char* text, int ii);
vector* get_tokens(const char* text);
void print_cmds(vector* cmds);
int is_operator(const char* token);


#endif
