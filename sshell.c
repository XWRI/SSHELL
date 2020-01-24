//
//  main.c
//  P1
//
//  Created by ZhangVito on 1/20/20.
//  Copyright © 2020 Joe. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMDLINE_MAX 512
#define ARG_MAX 16

typedef struct {
        char *cmd[ARG_MAX];
        int numArg;
} Process;

struct StackNode {
        char str[CMDLINE_MAX];
        struct StackNode* next;
};

struct StackNode* newNode(char* str) {
        struct StackNode* stackNode = (struct StackNode*)malloc(sizeof(struct StackNode));
        strcpy (stackNode->str, str);
        stackNode->next = NULL;
        return stackNode;
}

int isEmpty(struct StackNode* root) {
        if(!root) return 1;
        else return 0;
}

void push(struct StackNode** root, char* str) {
        struct StackNode* pointer = *root;
        if (pointer == NULL) {
                *root = newNode(str);
        } else {
                while(pointer->next != NULL) {
                        pointer = pointer->next;
                }
                pointer->next = newNode(str);
        }
}

int pop(struct StackNode** root) {
        if (isEmpty(*root)) return 0;
        struct StackNode* temp = *root;
        *root = (*root)->next;
        free(temp);
        return 1;
}

char* peek(struct StackNode* root) {
        return root->str;
}

void printStack(struct StackNode* root) {
        while(root != NULL) {
                printf("%s\n", root->str);
                root = root->next;
        }
}

int main(void)
{
        char input[CMDLINE_MAX];
        char buffer[CMDLINE_MAX];
        char cwd[CMDLINE_MAX];
        
        //declaring processes, each process have each individual cmd and arguments.
        Process processes[ARG_MAX];
        int pNum;
        char dirSymbol[ARG_MAX];
        int errorRedirect[ARG_MAX];
        int symbolNum;
        
        struct StackNode* root = NULL;

        //Main loop--------------------------------------------------------------------------
        while (1) {
                char *nl;
                int retval = 0;
                int val[ARG_MAX];
                
                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);
                
                /* Get command line */
                fgets(input, CMDLINE_MAX, stdin);
                
                /* Remove trailing newline from command line */
                if (!strcmp(input, "\n")) continue;
                nl = strchr(input, '\n');
                if (nl)
                        *nl = '\0';
                
                
                
                
                //Parsing all the commands and processes------------------------
                strcpy(buffer, input);
                int i, start = 0;
                int numRedirect = 0;
                int letterFound = 0;
                pNum = 0;
                symbolNum = 0;
                for (i = 0; i < CMDLINE_MAX; ++i) {
                        if (input[i] == '>' || input[i] == '|' || input[i] == '\0') {
                                if (!letterFound) { //empty argumnet between symbols.
                                        retval = 1;
                                        break;
                                }
                                if (input[i] != '\0') {
                                        dirSymbol[symbolNum] = input[i];
                                        if (input[i+1] == '&') {
                                                errorRedirect[symbolNum] = 1;
                                                buffer[i] = '\0';
                                                ++i;
                                        } else {
                                                errorRedirect[symbolNum] = 0;
                                        }
                                        symbolNum ++;
                                        buffer[i] = '\0';
                                }
                                if (input[i] == '>') numRedirect++;
                                //parse the substring into arguments
                                char* token = strtok(buffer+start, " ");
                                for (processes[pNum].numArg = 0; token != NULL; ++processes[pNum].numArg) {
                                        processes[pNum].cmd[processes[pNum].numArg] = token;
                                        token = strtok(NULL, " ");
                                }
                                processes[pNum].cmd[processes[pNum].numArg] = NULL;
                                start = i + 1;
                                pNum++;
                                letterFound = 0;
                                if (input[i] == '\0') break;
                        } else {
                                if (input[i]!=' ') letterFound = 1;
                        }
                }
                
                //checking errors of command line parsing.
                if (retval == 1) {//Found empty argumnet between symbols or at both ends.
                        if (numRedirect == 1 && input[i] == '\0' && dirSymbol[symbolNum-1] == '>') {
                                // "process.... > ", no output file after the > symbol.
                                fprintf(stderr, "Error: no output file\n");
                        } else {
                                fprintf(stderr, "Error: missing command\n");
                        }
                        continue;
                } else {
                        if (numRedirect > 1 || (numRedirect==1 && dirSymbol[symbolNum-1] != '>')) {
                                fprintf(stderr, "Error: mislocated output redirection\n");
                                retval = 1;
                                continue;
                        }
                }
                //Parsing finished----------------------------------------------
                
                
                
                
                
                // Print command line if stdin is not provided by terminal
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", input);
                        fflush(stdout);
                }
                
                if (pNum == 1) {
                        // Builtin command
                        if (!strcmp(processes[0].cmd[0], "exit")) {
                                fprintf(stdout, "Bye...\n");
                                break;
                        }
                        
                        // ls cmd
                        else if (!strcmp(processes[0].cmd[0], "ls")) {
                                if (processes[0].numArg > 2) {
                                        fprintf(stderr, "Error: too many process arguments\n");
                                        retval = 1;
                                        continue;
                                }
                                if (fork() != 0) {
                                        waitpid(-1, &retval, 0);        // wait for child to exit
                                } else {
                                        execvp(processes[0].cmd[0], processes[0].cmd);
                                        fprintf(stderr, "ls: cannot access '%s': No such file or directory\n", processes[0].cmd[1]);
                                        exit(1);
                                }
                        }
                        
                        // pwd cmd
                        else if (!strcmp(processes[0].cmd[0], "pwd")) {
                                getcwd(cwd, CMDLINE_MAX);
                                printf("%s\n", cwd);
                        }
                        
                        // cd cmd
                        else if (!strcmp(processes[0].cmd[0], "cd")) {
                                if (processes[0].cmd[1] == NULL) {
                                        chdir(getenv("HOME"));
                                } else if ((!strcmp(processes[0].cmd[1], ".")) && processes[0].cmd[2] == NULL) {
                                        //do nothing
                                } else if ((!strcmp(processes[0].cmd[1], "..")) && processes[0].cmd[2] == NULL) {
                                        chdir("..");
                                } else {
                                        if (chdir(processes[0].cmd[1]) != 0) {
                                                fprintf(stderr, "Error: no such directory\n");
                                                retval = 1;
                                                //fprintf(stderr, "+ completed '%s' [%d]\n", input, retval);
                                        }
                                }
                        }
                        
                        else if(!strcmp(processes[0].cmd[0], "dirs")) {
                                if(isEmpty(root)) {
                                        getcwd(cwd, CMDLINE_MAX);
                                        push(&root, cwd);
                                }
                                printStack(root);
                        }
                        
                        else if(!strcmp(processes[0].cmd[0], "popd")) {
                                if(!isEmpty(root)) {
                                        chdir(peek(root));
                                        pop(&root);
                                }
                                else {
                                        printf("Error: directory stack empty\n");
                                        retval = 1;
                                }
                        }
                        
                        else if(!strcmp(processes[0].cmd[0], "pushd")) {
                                if ((!strcmp(processes[0].cmd[1], "..")) && processes[0].cmd[2] == NULL) {
                                        chdir("..");
                                        getcwd(cwd, CMDLINE_MAX);
                                        push(&root, cwd);
                                } else if ((!strcmp(processes[0].cmd[1], ".")) && processes[0].cmd[2] == NULL) {
                                        getcwd(cwd, CMDLINE_MAX);
                                        push(&root, cwd);
                                } else {
                                        if (chdir(processes[0].cmd[1]) != 0) {
                                                fprintf(stderr, "Error: no such directory\n");
                                                retval = 1;
                                        } else {
                                                getcwd(cwd, CMDLINE_MAX);
                                                push(&root, cwd);
                                        }
                                }
                        }
                        
                        else {
                                if (fork() != 0) {
                                        waitpid(-1, &retval, 0);        // wait for child to exit
                                } else {
                                        execvp(processes[0].cmd[0], processes[0].cmd);
                                        // coming back here is an error
                                        fprintf(stderr, "Error: command not found\n");
                                        exit(1);
                                }
                        }
                }
                
                //Output Redirection--------------------------------------------
                else if (dirSymbol[0] == '>') {
                        int fd = open(processes[1].cmd[0], O_RDWR);
                        if (fd ==-1 || processes[1].numArg != 1) {
                                fprintf(stderr, "Error: cannot open output file\n");
                                continue;
                        }
                        if (fork() != 0) {
                                waitpid(-1, &retval, 0);        // wait for child to exit
                        } else {
                                if (fork() != 0) {
                                        waitpid(-1, &retval, 0);        // wait for child to exit
                                        if (retval != 0 && errorRedirect[0]) {
                                                dup2(fd, STDERR_FILENO);
                                                execvp(processes[0].cmd[0], processes[0].cmd);
                                                perror("execv");                // coming back here is an error
                                                exit(1);
                                        }
                                        exit(retval);
                                } else {
                                        dup2(fd, STDOUT_FILENO);
                                        execvp(processes[0].cmd[0], processes[0].cmd);
                                        exit(1);
                                }
                        }
                }
                
                
                //piping--------------------------------------------------------
                /*
                else {

                        
                        if (fork() != 0) {
                                waitpid(-1, &retval, 0);        // wait for child to exit
                        } else {
                                int fd[2];
                                int fd0 = 0;
                                pid_t pid;
                                //pipe(fd);
                                for (int i = 0; i < pNum; ++i) {
                                        pipe(fd);
                                        pid  = fork();
                                        if (pid != 0) {  //parent, execute
                                                if (i != pNum) {
                                                        dup2(fd0, STDOUT_FILENO);// Replace stdout
                                                        close(fd0);
                                                }
                 
                                                dup2(fd[1], STDIN_FILENO);// replace stdin with the pipe
                                                fd0 = fd[0];
                                                //close(fd[0]);         // Close now unused file descriptor
                                                close(fd[1]);
                 
                                                 
                                                if (fd[1] != 1) {
                                                        dup2(fd[1], 1);
                                                        close(fd[1]);
                                                }
                                                
                                                execvp(processes[i].cmd[0], processes[i].cmd);
                                                exit(1);
                                        }
                                        waitpid(-1, &retval, 0);
                                        fd0 = fd[0];
                                }
                                if(fork() == 0) {
                                        if(fd0 != 0) dup2(fd0, 0);
                                        execvp(processes[pNum-1].cmd[0], processes[pNum -1].cmd);
                                        exit(1);
                                } else {
                                        waitpid(-1, &retval, 0);
                                }
                                exit(0);
                        }
                                
                         
                                
                        
                 
                        }
                
                }
                 */
                
                else {
                        
                        int in = 0;
                        int fd[2];
                        
                        
                        for(int i = 0; i < pNum-1; i++) {
                                
                                pipe(fd);
                                if(fork() == 0) { //child, execute
                                        
                                        // Connect pipe.
                                        if (in != 0) {
                                                dup2(in, 0);
                                                close(in);
                                        }
                                        if (fd[1] != 1) {
                                                dup2(fd[1], 1);
                                                close(fd[1]);
                                        }
                                        execvp(processes[i].cmd[0], processes[i].cmd);
                                        exit(1);
                                }
                                
                                waitpid(-1, &retval, 0);
                                val[i] = retval;
                                close(fd[1]);
                                in = fd[0];
                        }
                        
                        if(fork() == 0) {
                                if(in != 0) dup2(in, 0);
                                execvp(processes[pNum-1].cmd[0], processes[pNum -1].cmd);
                                exit(1);
                        } else {
                                waitpid(-1, &retval, 0);
                        }
                                
                        
                }
                                 
                if(pNum > 1) {
                        fprintf(stdout, "+ completed '%s' ", input);
                        for (int i = 0; i < pNum; i++) {
                                fprintf(stdout, "[%d]", val[i]);
                        }
                        fprintf(stdout, "\n");
                } else {
                        fprintf(stdout, "+ completed '%s' [%d]", input, retval);
                }
                
        }
        
        return EXIT_SUCCESS;
}
