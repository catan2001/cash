#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

char pcmd[100];

void _parsecmd(char *str, char cmd[]) {
    int i = 0;    

    while(*(str+i) != 0) {
        if(str[i] == ' ') cmd[i] = 0;
        else
            cmd[i] = str[i];
        i++;
    } 
}

int main(int argc, char **argv) {
    // TODO: 
    // 1) Execute file but stay in cash!
    // 2) mkdir
    // 3) go thru file system
    // 4) parse commands 
    // 5) 
 
    if(argc == 1) { 
        printf("ERROR: nothing parsed\n");
        return -1;
    }

    _parsecmd((*(argv+1)), pcmd);
    printf("%s\n", pcmd);
    printf("%s\n", *(argv+1)+1);
    strcpy(pcmd, *(argv+1));
    printf("%s\n", pcmd+2);

    if(pcmd[0] == '.' && pcmd[1] == '/') {
        if(execl(pcmd+2, pcmd+2, NULL) == -1) {
            printf("ERROR: could not run executable\n");
            return -1;
        }
    }

    printf("Check\n");
    return 0;
}
