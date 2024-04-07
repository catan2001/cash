#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>

char pcmd[100];
#define TRUE 1
#define FALSE 0
#define ERROR -1

void clrs(void) {
    system("clear");
}

char **tokenizer(char *cmd) {
    //what if empty??
    int i = 0;
    int word_cnt = 1;
    for(;;) {
        if(*(cmd + i) == 0) break;
        if(*(cmd + i) == ' ') word_cnt++;
        i++;
    }

    char **tokens = malloc(word_cnt * sizeof(char*));
    if(tokens == NULL) {
        fprintf(stderr, "ERROR: tokenizer [could not allocate memory]\n");
        exit(1);
    }

    i = 0;
    word_cnt = 0;
    char *token = cmd;
    tokens[word_cnt] = token;
    // tokenizes the string
    for(;;) {
        if(*(cmd + i) == 0) break;
        if(*(cmd + i) == ' ' && *(cmd+i+1) != ' ') {
            cmd[i] = '\0'; // replace whitespace with null terminator
            word_cnt++;
            token = (cmd + i + 1); // reference next string 
            tokens[word_cnt] = token;
        }
        i++;
    }
    tokens[word_cnt+1] = NULL; // make last NULL just in case of counting the number of tokens;
    return tokens;
}

// TODO: ./execution does not work
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
// Lexer...
/*
const char * const parse_cmd(int *argc, char **argv) {
    if(*argc > 0) {
        char * key_word = *argv;
        *argv += 1;
        *argc -= 1;
        return key_word; 
    }
    return NULL;
}
*/


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
    /*  
    char test[] = "test best mjau";
    char **words = tokenizer(test);

    // Print each word
    for (int i = 0; words[i] != NULL; i++) {
        printf("%s\n", words[i]);
    }

    // Free memory
   free(words);
    */

    while(TRUE) {
        read_cmd(pcmd, sizeof(pcmd));

        char **words = tokenizer(pcmd);
        // Print each word
        for (int i = 0; words[i] != NULL; i++) {
            print_term(words[i]);
            print_term("\n");
        }

        free(words);
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
    
