#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<sys/wait.h>

char pcmd[100];

#define TRUE 1
#define FALSE 0
#define ERROR -1

void clrs(void) {
    system("clear");
}

void exec(char *cmd) {
    int len;
    if(cmd[0] == '.' && cmd[1] == '.') {
        len = strlen(cmd+2); // for some reason it's not NULL terminated...
        cmd[len+1] = 0;

        if(execl(cmd+2, cmd+2, NULL) == -1) {
            fprintf(stderr, "Could not execute file\n");
        }
    }
    if(cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 'r' && cmd[3] == 's') clrs();
}

void parse_cmd(int argc, char **argv, char cmd[]) {
    int i = 0, k = 0;  
    int j = 1; // used to skip first argument
    while(*(*(argv+j)+i) != 0) {
        //printf("%d: %c\n", j, *(*(argv+j)+i));
        cmd[k] = *(*(argv+j)+i);
        if(*(*(argv+j)+i+1) == 0 && argc > j+1) { // check if the next char is NULL and if j <= than number of args
            cmd[k+1] = ' '; 
            j++;
            k++;
            i = 0;
            //printf("entered\n");
        }   
        else
            ++i;
        //printf("cmd: %c\n", cmd[k]);
        k++;
    }
    cmd[k] = 0;
    printf("test: %s\n", cmd);
}

void read_cmd(char *pcmd, int lcmd) {
    if(isatty(fileno(stdin)))
        fprintf(stdout, "cash> ");
    memset(pcmd, 0, lcmd);
    fgets(pcmd, lcmd, stdin);
}

int print_term(char *msg) {
    if(isatty(fileno(stdin))) {
        fprintf(stdout, msg);
        return 0;
    }
    
    fprintf(stderr, "Could not output to terminal, file descriptor %d", fileno(stdin));
    return -1;
}

int main(void) {
    // TODO: 
    // 1) Execute file in cash! ...
    // 2) mkdir ...
    // 3) go thru file system ...
    // 4) parse commands from stdin ...
    // 5) cash> prompt sign ... done
    // 6) while until exits ... done
    // 7) implement clrs for clear screen ... done
    // 8) execute file but stay in cash! ...
    
    while(TRUE) {
        read_cmd(pcmd, sizeof(pcmd));
        exec(pcmd);
        wait(NULL);
    }
/*

    _parsecmd(argc, argv, pcmd);
    printf("%s\n", pcmd);

    if(pcmd[0] == '.' && pcmd[1] == '/') {
        if(execl(pcmd+2, pcmd+2, NULL) == -1) {
            fprintf(stdout, "ERROR: could not run executable\n");
            //return -1;
        }
    }
*/
    return 0;
}
