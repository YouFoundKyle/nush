#include "vector.h"
#include "tokens.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Functions for reading tokens */


// checks if the inputted char is an operator
int 
is_operator(const char* token)
{
    
/*
Seven Operators: < > | & && || ; 
Redirect input: sort < foo.txt              1
Redirect output: sort foo.txt > output.txt  2
Pipe: sort foo.txt | uniq                   3
Background: sleep 10 &                      4
Semicolon: echo one; echo two               5
And: true && echo one                       6
Or: true || echo one                        7
 */

    /* Get first Char */
    char* tok = malloc(sizeof(char) + 1);
    memcpy(tok, token, sizeof(char));
    tok[sizeof(char)] = 0;
    
    /* Seperated into different statements since I think that will be useful for the challenge */
    if (strcmp(tok,"<")==0){
        free(tok);
        return 1;
    }
    else if (strcmp(tok,">")==0){
        free(tok);
        return 2;
    }
    else if (strcmp(tok,"|")==0){
        free(tok);
        return 3;
    }
    else if (strcmp(tok,"&")==0){
        free(tok);
        return 4;
    }
    else if (strcmp(tok,";")==0){
        free(tok);
        return 5;
    }
    else {
        free(tok);
        return 0;
    }   
    }

/* Checks if its an operator made up of 2 chars*/
int 
is_long_operator(const char* token)
{
    
/*
Seven Operators: < > | & && || ; 
And: true && echo one
Or: true || echo one
*/

    /* Get first 2 chars */
    char* tok = malloc((sizeof(char)*2) + 1);
    memcpy(tok, token, (sizeof(char)*2));
    tok[(sizeof(char)*2)] = 0;
    
    /* Seperated into different statements since I think that will be useful for the challenge */
    if (strcmp(tok,"||")==0){
        free(tok);
        return 6;
    }
    else if (strcmp(tok,"&&")==0){
        free(tok);
        return 7;
    }
    else {
        free(tok);
        return 0;
    }   
    }

/* Reads in operator from the inputed text */
char*
read_operator(const char* text, int ii)
{
    int nn = 1;
    if (is_long_operator( &(text[ii])) ) {
        char* num = malloc(3);
        memcpy(num, text + ii, 2);
        num[2] = 0;
        return num;
    }
    char* num = malloc(nn + 1);
    memcpy(num, text + ii, nn);
    num[nn] = 0;
    return num;
}

// Reads a token from the inputted text
char*
read_token(const char* text, int ii)
{
    int nn = 0;
    while ( !is_operator( &(text[ii + nn])) && !isspace(text[ii + nn])) {
        ++nn;
    }

    char* num = malloc(nn + 1);
    memcpy(num, text + ii, nn);
    num[nn] = 0;
    return num;
}

// Returns a vector with an array of all the tokens parsed from the inputted text
vector*
get_tokens(const char* text)
{
    vector* xs = make_vector();

    int nn = strlen(text);
    int ii = 0;
    while (ii < nn) {
        char* tok;
        if (isspace(text[ii])) {
            ++ii;
            continue;
        }

        if (!is_operator(&(text[ii]))) {
            tok = read_token(text, ii);
            vector_push(xs, tok);
            ii += strlen(tok);
            free(tok);
            continue;
        }
        else {
        tok = read_operator(text, ii);
        vector_push(xs, tok);
        ii += strlen(tok);
        free(tok);
        continue;
        }
    }

    return xs;
}

// Prints out the tokens within the vector
// Meant for debugging
void
print_cmds(vector* cmds){
    for (int ii = 0 ; ii <= cmds->size -1; ii++){
    char* tok = cmds->data[ii];
    puts(tok);
    }
}




