#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1

#define MAX_SIZE 100
enum TOKEN_TYPE
{
    COMMAND,
    ARGUMENT,
    OPERATOR,
    CTRL_FLOW,
    COMMENT
};

enum TOKEN_VALUE
{
    PWD,
    CLEAR
};

typedef struct token_s
{
    enum TOKEN_TYPE type;
    char *value;
} token_t;

char pcmd[MAX_SIZE];

/*@read_cmd
**Function: Prints prompt and reads input from stdin*/
void read_cmd(char *pcmd, int lcmd)
{
    if (isatty(fileno(stdin)))
        fprintf(stdout, "cash> ");
    else
        return;
    memset(pcmd, 0, lcmd); // necessary?
    fgets(pcmd, lcmd, stdin);
}

/*@print_term
**Function: Prints output to prompt*/
int print_term(char *msg)
{
    if (isatty(fileno(stdin)))
    {
        fprintf(stdout, msg);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Could not output to terminal, file descriptor %d", fileno(stdin));
    return EXIT_FAILURE;
}

void clrs(void)
{
    system("clear");
}

int compare_token(char *token, char *value)
{
    if (strlen(token) != strlen(value))
        return EXIT_FAILURE;
    for (int i = 0; token[i] != 0; ++i)
    {
        if (token[i] != value[i])
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

char **tokenizer(char *cmd)
{
    if (cmd[0] == 0 || cmd[0] == '\n')
        return NULL;

    int word_cnt = 0;
    int capacity = 2;
    char **tokens = (char **)malloc(capacity * sizeof(char *));
    if (tokens == NULL)
    {
        fprintf(stderr, "ERROR: tokenizer [could not allocate memory]\n");
        exit(EXIT_FAILURE);
    }

    char *token = cmd;
    for (int i = 0; cmd[i] != '\0'; ++i)
    {
        if (cmd[i] != ' ' && cmd[i] != '\n')
            continue;
        cmd[i] = '\0'; // replace white space with null terminator

        if (word_cnt >= capacity) // reallocate larger memory...
        {
            capacity *= 2;
            char **new_tokens = realloc(tokens, capacity * sizeof(char *));
            if (!new_tokens) // deallocate if fail
            {
                fprintf(stderr, "ERROR: tokenizer [realloc failed]\n");
                for (int j = 0; j < word_cnt; ++j)
                    free(tokens[j]);
                free(tokens);
                exit(EXIT_FAILURE);
            }
            tokens = new_tokens;
        }
        tokens[word_cnt++] = strdup(token); // Copy token into the array

        while (cmd[++i] == ' '); // skip spaces
        token = &cmd[i]; // fetch next token
    }
    return tokens;
}

token_t *classify_tokens(char **token)
{
    token_t *ctoken = (token_t *)malloc(sizeof(token_t));
    for (int i = 0; token[i] != NULL; i++)
    {
        ctoken = (token_t *)realloc(ctoken, sizeof(token_t) * (i + 1) * 2);
        if (!compare_token(token[i], "pwd"))
        {
            ctoken[i].type = COMMAND;
            ctoken[i].value = "pwd";
        } else
        if (!compare_token(token[i], "clear") || !compare_token(token[i], "clrs"))
        {
            ctoken[i].type = COMMAND;
            ctoken[i].value = "clear";  
        }
        else
        {
            ctoken[i].type = ARGUMENT;
            ctoken[i].value = token[i];
        }
        printf("TOKEN: ");
        print_term(ctoken[i].value);
        print_term("\n");
    }
    return ctoken;
}

// TODO: parse the tokens into a tree...

/* EXAMPLE: cmd1 | cmd2 > file

         |
       /   \
   cmd1     >
          /   \
       cmd2   file

    RULE: execute always first left branch
*/

/* for(int i = 0; ctox != NULL; ++i) {

}

*/

// TODO: ./execution does not work
void exec(char *cmd)
{
    int len;
    if (cmd[0] == '.' && cmd[1] == '.')
    {
        len = strlen(cmd + 2); // for some reason it's not NULL terminated...
        cmd[len + 1] = 0;
        if (execl(cmd + 2, cmd + 2, NULL) == -1)
        {
            fprintf(stderr, "Could not execute file\n");
        }
    }
}

int main(void)
{
    clrs();

    while (TRUE)
    {
        read_cmd(pcmd, sizeof(pcmd));
        char **tokens = tokenizer(pcmd);
        token_t *ctox = classify_tokens(tokens);

        free(tokens); // add for loop to properly free  
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
