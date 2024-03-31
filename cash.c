#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

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

// TODO: implement parsing similar to Bash's one
const char * const parse_cmd(int *, char **);

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

    while(TRUE) {
        read_cmd(pcmd, sizeof(pcmd));
        exec(pcmd);
        wait(NULL);
    }

    return 0;
}

    // TODO: 
    // 1) Execute in cash! ... done
    // 2) mkdir ...
    // 3) go thru file system ...
    // 4) parse commands from stdin ...
    // 5) cash> prompt sign ... done
    // 6) while until exits ... done
    // 7) implement clrs for clear screen ... done
    // 8) execute file but stay in cash! ...
    
