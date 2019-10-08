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
        char *ptr = input;
        char *dir;
        char *gdir;
        char *to;
        char buf[1000];
        char *args[MAXARGS + 1] = { NULL };

        // Read Input
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

        // Quit Bash
        if (!strcmp(args[0], "quit")) exit(0);

        // cd command - switch directory
        if (!strcmp(args[0], "cd")){
          gdir = getcwd(buf, sizeof(buf));
          dir = strcat(gdir, "/");
          if(args[1] == 0){
            printf("%s\n",gdir);
          }
          else{
          to = strcat(dir, args[1]);
          if(chdir(to) == -1) printf("%s","error: no such directory found\n");
          }
        }

        // clear shell
        if (!strcmp(args[0], "clr")){
            clear();
        }

        //Execute
        signal(SIGINT, SIG_DFL);
        if (fork() == 0) {
          exit(execvp(args[0], args));
        }
        signal(SIGINT, SIG_IGN);
        wait(NULL);
    }
}
