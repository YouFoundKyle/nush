
# nush
A light Unix Shell


*nush* operates as follows:

*   Accept an optional script file as the first command line argument.
*   If there's no script, show the prompt "nush$". Your prompt should be "nush$ ". Command input is accepted on the same line after the prompt.
*   Read one line of commands from either stdin or the script.
*   Execute that line of commands.
*   Repeat until EOF or an "exit" command.


Given a program to execute, like "head" above, the shell does three things:

*   fork(), to create a new process.
*   in the child process, exec(), to execute the requested program, passing through the three command line arguments
*   in the parent process, wait() or waitpid(), to allow the program to finish before executing another command



## Built-in Commands
*   The "cd" command changes the current directory.
*   The "exit" command exits the shell.


# Shell Operators:

Operators

    nush$ sort sample.txt | tail -n 3
    apples
    nonchalant     
    underplays
    nush$ 

Operators either modify the behavior of one command or chain together multiple commands into a single command line.

*   Redirect input: sort < foo.txt
*   Redirect output: sort foo.txt > output.txt
*   Pipe: sort foo.txt | uniq
*   Background: sleep 10 &
*   And: true && echo one
*   Or: true || echo one
*   Semicolon: echo one; echo two

