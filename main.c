#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define PRMTSIZ 255
#define MAXARGS 63
#define EXITCMD "exit"
#define clear() printf("\033[H\033[J")

int main(void) {
    clear();
    printf("%s\n","Welcome to Ben's Bash!");
    while (1) {

        // Arrays for storing input
        char input[PRMTSIZ + 1] = { 0x0 };
        char *inputPipe[PRMTSIZ];
        char *outputPipe[PRMTSIZ];
        char *ptr = input;
        char *dir;
        char *gdir;
        char *to;
        int isPiping = 0;
        char buf[1000];
        char *args[MAXARGS + 1] = { NULL };
        gdir = getcwd(buf, sizeof(buf));

        // Read Input
        printf("%s",gdir);
        printf("%s ", "$");
        fgets(input, PRMTSIZ, stdin);

        // Parse Input
        if (*ptr == '\n') continue;
        for (int i = 0; i < sizeof(args) && *ptr; ptr++) {
            if (*ptr == ' ') continue;
            if (*ptr == '\n') break;
            for (args[i++] = ptr; *ptr && *ptr != ' ' && *ptr != '\n'; ptr++);
            *ptr = '\0';
        }

        // Check for Multiple Commands
        if (!strcmp(args[0], "date;cal;")){
            printf("%s\n","yeeeet");
            char* temp = args[0];
            for (int i = 0; i < strlen(temp); i++){
                if(temp[i] == ';'){
                  printf("%c\n",temp[i]);
                }
            }
        }
        // Quit Bash
        if (!strcmp(args[0], "quit")){
        exit(0);
        }
        // Switch Directory
        if (!strcmp(args[0], "cd")){
          dir = strcat(gdir, "/");
          if(args[1] == 0){
            printf("%s\n",gdir);
          }
          else{
          to = strcat(dir, args[1]);
          if(chdir(to) == -1) printf("%s","error: no such directory found\n");
          }
        }

        // Clear shell
        if (!strcmp(args[0], "clr")){
            clear();
        }

        // Print environement strings
        if (!strcmp(args[0], "environ")){
            args[0] = "printenv";
        }

        // Print more menu
        if (!strcmp(args[0], "help")){
            args[0] = "more";
        }

        //Piping
        if(isPiping == 1){
            // 0 is read end, 1 is write end
            int pipefd[2];
            pid_t p1, p2;

            if (pipe(pipefd) < 0) {
                printf("\nPipe could not be initialized");
            }
            p1 = fork();
            if (p1 < 0) {
                printf("\nCould not fork");
            }
            if (p1 == 0) {
                // Child 1 executing..
                // It only needs to write at the write end
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);

                if (execvp(args[0], args) < 0) {
                    printf("\nCould not execute command 1..");
                    exit(0);
                }
            } else {
                // Parent executing
                p2 = fork();

                if (p2 < 0) {
                    printf("\nCould not fork");
                }

                // Child 2 executing..
                // It only needs to read at the read end
                if (p2 == 0) {
                    close(pipefd[1]);
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);
                    if (execvp(args[0], args) < 0) {
                        printf("\nCould not execute command 2..");
                        exit(0);
                    }
                } else {
                    // parent executing, waiting for two children
                    wait(NULL);
                    wait(NULL);
                }
              }
            }
        else{
        //Execute
        signal(SIGINT, SIG_DFL);
        if (fork() == 0) {
          exit(execvp(args[0], args));
        }
        signal(SIGINT, SIG_IGN);
        wait(NULL);
        }
    }
}
