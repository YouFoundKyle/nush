#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "vector.h"
#include "tokens.h"

/*declaring function so gcc doesn't yell */
int parse_cmds(vector* cmds);

// changes the directory that the shell is operating in
int
execute_cd(char* args){
    chdir(args);
    return 1;
}

// exits from the shell
int
execute_exit(){
    exit(0);
    return 1;
    
}

// Checks if the command inputted is a built in one
// The builtin commands are: cd and exit
int
is_builtin(char* cmd, char** args){
    if (strcmp(cmd,"cd")==0){
         return execute_cd(args[1]);
         
        
    }
    else if (strcmp(cmd,"exit")==0){
        return execute_exit();
        
    }
    else{
        return 0;
    }
    
}

// Executes a command with no operators
int
execute(vector* cmd)
{
    int cpid;
    char* args[30];
    char* command = cmd->data[0];
    args[0] = command;
    
    for (int ii = 1; ii < cmd->size; ii++){
    args[ii] = vector_get(cmd, ii);
    }
    
    args[cmd->size] = 0;
    
    int built_in =is_builtin(command, args);
    if (!built_in){
        if ((cpid = fork())) {
            // parent process
            // Child may still be running until we wait.

            int status;
            waitpid(cpid, &status, 0);
            
            return status;

        }
        else {
            // child process
            
            // The argv array for the child.
            // Terminated by a null pointer.

            execvp(command, args);
            assert(0);
            }
    }
    else{
        return built_in;
    }
}

// Executes a redirect from the input file to the first command 
// For use with the < operator
int 
execute_redirect_input(vector* cmd1, vector* inp)
{

    int cpid;
 
    char* args[30];
    
    char* command = cmd1->data[0];
    
    
    args[0] = command;
    
   args[1] = inp->data[0];
    
    args[2] = 0;
        
        
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
        return status;
    }
    else {
        execvp(command, args);
    }
}

// Executes a redirect from the first command to the output file
// For use with the > operator

int 
execute_redirect_output(vector* cmd1, vector* out){

    int cpid;
 
    char* args[30];
    
    char* command = cmd1->data[0];
    
    
    args[0] = command;
    
    for (int ii = 1; ii < cmd1->size; ii++){
    args[ii] = vector_get(cmd1, ii);
    }
    
    args[cmd1->size] = 0;
        
        
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
        return status;
    }
    else {
        int fd = open(out->data[0], O_CREAT | O_APPEND | O_WRONLY, 0644);
        close(1);
        dup(fd);
        close(fd);

        execvp(command, args);
    }
    
}

// Executes a set of commands that require piping
// Used witht the || Operator
int 
execute_pipe(vector* cmd1, vector* cmd2){
    int cpid, cpid2,  rv;

    /* Sets Up args for execvp */
    char* args[30];
    char* command = cmd1->data[0];
    args[0] = command;
    for (int ii = 1; ii < cmd1->size; ii++){
    args[ii] = vector_get(cmd1, ii);
    }
    args[cmd1->size] = 0;
    
    /* set up pipes */
    int pipes[2];
    rv = pipe(pipes);
    assert(rv != -1);

    // pipes[0] is for reading
    // pipes[1] is for writing

    
    if(cpid = fork())
    {
        int status;
        waitpid(cpid, &status, 0);
        close(pipes[0]);
        close(pipes[1]);
        wait(0);

        return status;
    }
    else 
    {
        if(cpid2 = fork()) {
        // child
        close(pipes[0]); //close the read end of the pipe 
        close(1); // close stdout ... is now an empty slot in the FD table
        rv = dup(pipes[1]); //set stdout as the write end of pipe i.e redirecting it so the stdout is the pipe 
        assert(rv != -1); //error checking
        
        close(pipes[1]); //close the right of of the pipe so you dont end up with duplicates
        
        execvp(command, args); //this will execute sort and output it to the pipe
        assert(0); //this should neer run since sort is running and exec shouldnt fail

        }
        else {

            // child-child
            close(pipes[1]); //close the read end of the pipe 
            close(0); // close stdout ... is now an empty slot in the FD table
            rv = dup(pipes[0]); //set stdout as the write end of pipe i.e redirecting it so the stdout is the pipe 
            assert(rv != -1); //error checking
            
            close(pipes[1]); //close the right of of the pipe so you dont end up with duplicates

            parse_cmds(cmd2);    
        }
    }

}

// Executes a command in the background of the shell
// For use of the & operator
int
execute_background(vector* cmd){
    int cpid;
    char* args[30];
    char* command = cmd->data[0];
    args[0] = command;
    
    for (int ii = 1; ii < cmd->size; ii++){
    args[ii] = vector_get(cmd, ii);
    }
    
    args[cmd->size] = 0;
    

    if (cpid = fork()){
        execvp(command, args);
        assert(0);
    }
    
    int status;
    waitpid(cpid, &status, 0);
    return status;
}

// executes a set of commands witht the && operator
int 
execute_and(vector* cmd1, vector* cmd2){

    if (parse_cmds(cmd1) == 0){
        return parse_cmds(cmd2);
    }
    else {
        return 1;
    }
    
}

// executes a set of commands witht the || operator
int 
execute_or(vector* cmd1, vector* cmd2){    
    if (parse_cmds(cmd1) == 0){
        return 0;
    }
    else {
        return parse_cmds(cmd2);
    }
    
}


int
parse_cmds(vector* cmds){
/*
Seven Operators: < > | & && || ; 
Redirect input: sort < foo.txt              1
Redirect output: sort foo.txt > output.txt  2
Pipe: sort foo.txt | uniq                   3
Background: sleep 10 &                      4
Semicolon: echo one; echo two               5
Or: true || echo one                        6
And: true && echo one                       7
 */


    /* Find the index of the first operator */
    int nn = 0;
    while ( !is_operator( vector_get(cmds, nn)) && !is_long_operator(vector_get(cmds, nn))) {
        if (nn < cmds->size -1){
        nn++;
        }
        else {
        nn = 0;
        break;
            
        }
    }
    
    int op;
    
    /* Set the Operator code of the first operator in the cmd sequence */
    if (is_long_operator(vector_get(cmds, nn))) {
    op = is_long_operator(vector_get(cmds, nn)) ;
    }
    else {
    op = is_operator( vector_get(cmds, nn));
        
    }
    
    
//     printf("Operator # %d is at index %d\n", op, nn);
    
    vector* cmd1 = make_vector();
    vector* cmd2 = make_vector();
    
    /* Save the first command in the sequence */
    for (int ii = 0; ii < nn; ii++){
    vector_push(cmd1, cmds->data[ii]);
    }
    
    /* save all subsequent commands */
    for (int jj = nn+1; jj < cmds->size; jj++){
    vector_push(cmd2, cmds->data[jj]);
    }
    
    int status = 1;
    switch(op){
        case 0: 
            status =  execute(cmds);
            break;
        case 1:
            status = execute_redirect_input(cmd1, cmd2);
            break;
        case 2:
            status = execute_redirect_output(cmd1, cmd2);
            break;
        case 3:
            status = execute_pipe(cmd1, cmd2);
            break;
        case 4:
            status = execute_background(cmd1);
            break;
        case 5:
            parse_cmds(cmd1);
            parse_cmds(cmd2);
            status = 0;
            break;
        case 6:
            status = execute_or(cmd1, cmd2);
            break;
        case 7:
            status = execute_and(cmd1, cmd2);
            break;

    }
    
    free_vector(cmd1);
    free_vector(cmd2);
    
    return status;
}



int
main(int argc, char* argv[])
{
    char cmd[256];

    if (argc == 1) {
        printf("nush$ ");
        while (fgets(cmd, 256, stdin) != NULL) {
            fflush(stdout);
            vector* cmds = get_tokens(cmd);
            parse_cmds(cmds);
            printf("nush$ ");
            free_vector(cmds);
        }
    }

    else {
        fflush(stdout);
        FILE *f = fopen(argv[1], "r");
        while(fgets(cmd, 256, f) != NULL) {
            vector* cmds = get_tokens(cmd);
            parse_cmds(cmds);
            free_vector(cmds); 
        }
    }


    return 0;
}
