#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define PRMTSIZ 255
#define MAXARGS 63
#define EXITCMD "exit"
#define clear() printf("\033[H\033[J")


// function for checking if command contains pipe and then sets input/output sides
int parsePipe(char* str, char** strpiped)
{
    if(strchr(str, '|') == NULL){
      return 0;
    }
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }

    if (strpiped[1] == NULL)
        return 0; // returns zero if no pipe is found.
    else {
        return 1;
    }
}


// excute different system calls
void execute(int isPiping, char** ipargs, char** opargs, char** args, int multipleCommands, int numOfCommands, char newString[10][10]){
  //Piping
  if(isPiping == 1){
      int pipefd[2];
      pid_t p1, p2;
      if (pipe(pipefd) < 0) printf("\nPipe could not be initialized");
      if ((p1 = fork()) < 0) printf("\nCould not fork");
      // first child
      if (p1 == 0) {
          close(pipefd[0]);
          dup2(pipefd[1], STDOUT_FILENO);
          close(pipefd[1]);
          exit(execvp(ipargs[0], ipargs));
      } else {
        // second child
          if ((p2= fork()) < 0) printf("\nCould not fork");
          if (p2 == 0) {
              close(pipefd[1]);
              dup2(pipefd[0], STDIN_FILENO);
              close(pipefd[0]);
              exit(execvp(opargs[0], opargs));
          } else {
              // parent executing, waiting for two children
              wait(NULL);
              wait(NULL);
          }
        }
      }
  // not piping, single command
  else if(multipleCommands == 0){
  if (fork() == 0) {
    exit(execvp(args[0], args));
  }
  wait(NULL);
  }

  //multiple commands
  else{
    for(int i = 0; i<numOfCommands; i++){
      if (fork() == 0) {
        exit(execvp(newString[i], args));
      }
    }
    for(int i=0;i<numOfCommands;i++) {
    wait(NULL);
  }
  }
}

int main(int argc, char *argv[]) {

    // read from file if arguments
    if(argc >= 2){
    FILE *fp;
    char *mode = "r";
    int valid=1;
    fp = fopen(argv[1], mode);
    while (valid == 1) {
      char input[PRMTSIZ + 1] = { 0x0 };
      // store arguments for multiple commands, input, output, and standard processes
      char newString[10][10];
      char *ipargs[MAXARGS + 1] = { NULL };
      char *opargs[MAXARGS + 1] = { NULL };
      char *args[MAXARGS + 1] = { NULL };

      // pointer for start of file line or command line
      char *ptr = input;

      // directory info
      char *dir;
      char *gdir;
      char *to;

      // ints to check which executing path to take. piped, single, or multiple commands
      int isPiping = 0;
      int multipleCommands = 0;
      int numOfCommands = 0;


      char buf[1000];
      gdir = getcwd(buf, sizeof(buf));

      // get line from file and set to input
      if(fgets(input, PRMTSIZ, fp) == NULL){
        fclose(fp);
        valid = 0;
      }
      if(valid == 1){
      // Check Piping and parse if there is a '|'
      char *strpiped[2];
      isPiping = parsePipe(input,strpiped);
      if(isPiping == 1){
      char *ip = strpiped[0];
      char *op = strpiped[1];

      //parse input args
      for (int i = 0; i < sizeof(ipargs) && *ip; ip++) {
          if (*ip == ' ') continue;
          if (*ip == '\n') break;
          for (ipargs[i++] = ip; *ip && *ip != ' ' && *ip != '\n'; ip++);
          *ip = '\0';
          }

      // Parse output args
      for (int i = 0; i < sizeof(opargs) && *op; op++) {
          if (*op == ' ') continue;
          if (*op == '\n') break;
          for (opargs[i++] = op; *op && *op != ' ' && *op != '\n'; op++);
          *op = '\0';
          }
    }

      // Parse Input
      for (int i = 0; i < sizeof(args) && *ptr; ptr++) {
          if (*ptr == ' ') continue;
          if (*ptr == '\n') break;
          for (args[i++] = ptr; *ptr && *ptr != ' ' && *ptr != '\n'; ptr++);
          *ptr = '\0';
      }

      // Check for Multiple Commands and parse into unqiue commands
      if(strchr(args[0], ';') != NULL){
      multipleCommands = 1;
      int val=0;
      int j=0;
      char *ip = args[0];
      while(ip[val]!='\0'){
          if(ip[val] == ';'){
                  newString[numOfCommands][j]='\0';
                  numOfCommands++;  //for next word
                  j=0;    //for next word, init index to 0
            } else {
                  newString[numOfCommands][j]=ip[val];
                  j++;
            }
          val++;
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


    // execute system calls depending on different variables
    execute(isPiping, ipargs, opargs, args, multipleCommands, numOfCommands, newString);
      }
    }
  }
  // If no parameters on bash, then take user input
  else{
    printf("%s\n","Welcome to Ben's Bash!");
    // continuously provide input
    while (1) {
      char input[PRMTSIZ + 1] = { 0x0 };
      // store arguments for multiple commands, input, output, and standard processes
      char newString[10][10];
      char *ipargs[MAXARGS + 1] = { NULL };
      char *opargs[MAXARGS + 1] = { NULL };
      char *args[MAXARGS + 1] = { NULL };

      // pointer for start of file line or command line
      char *ptr = input;

      // directory info
      char *dir;
      char *gdir;
      char *to;

      // ints to check which executing path to take. piped, single, or multiple commands
      int isPiping = 0;
      int multipleCommands = 0;
      int numOfCommands = 0;


      char buf[1000];
      gdir = getcwd(buf, sizeof(buf));

      // get stdin if not a file run
      printf("%s",gdir);
      printf("%s ", "$");
      fgets(input, PRMTSIZ, stdin);

      // Check Piping and parse if there is a '|'
      char *strpiped[2];
      isPiping = parsePipe(input,strpiped);
      if(isPiping == 1){
      char *ip = strpiped[0];
      char *op = strpiped[1];

      //parse input args
      for (int i = 0; i < sizeof(ipargs) && *ip; ip++) {
          if (*ip == ' ') continue;
          if (*ip == '\n') break;
          for (ipargs[i++] = ip; *ip && *ip != ' ' && *ip != '\n'; ip++);
          *ip = '\0';
          }

      // Parse output args
      for (int i = 0; i < sizeof(opargs) && *op; op++) {
          if (*op == ' ') continue;
          if (*op == '\n') break;
          for (opargs[i++] = op; *op && *op != ' ' && *op != '\n'; op++);
          *op = '\0';
          }
    }

      // Parse Input
      for (int i = 0; i < sizeof(args) && *ptr; ptr++) {
          if (*ptr == ' ') continue;
          if (*ptr == '\n') break;
          for (args[i++] = ptr; *ptr && *ptr != ' ' && *ptr != '\n'; ptr++);
          *ptr = '\0';
      }

      // Check for Multiple Commands and parse into unqiue commands
      if(strchr(args[0], ';') != NULL){
      multipleCommands = 1;
      int val=0;
      int j=0;
      char *ip = args[0];
      while(ip[val]!='\0'){
          if(ip[val] == ';'){
                  newString[numOfCommands][j]='\0';
                  numOfCommands++;  //for next word
                  j=0;    //for next word, init index to 0
            } else {
                  newString[numOfCommands][j]=ip[val];
                  j++;
            }
          val++;
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


      // execute system calls depending on different variables
    execute(isPiping, ipargs, opargs, args, multipleCommands, numOfCommands, newString);
      }
    }
}
