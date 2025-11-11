
#include "../../include/minishell.h"

// Define global signal variable
volatile sig_atomic_t g_signal = 0;

void print_lexer(t_lexer *lexer)
{
    t_lexer *tmp = lexer;
    
    int i = 0;

    while (tmp && tmp->tokentype != TOK_END)
    {
        if (!tmp->str)
        {
            printf("[ERROR] Node %d has NULL str\n", i);
        }
        else
        {
            printf("%s\n", tmp->str);
        }

        tmp = tmp->next;
        i++;
    }
}

// Free the lexer linked list
void free_lexer(t_lexer *lexer)
{
    t_lexer *tmp;
    while (lexer)
    {
        tmp = lexer;
        lexer = lexer->next;
        free(tmp->str); // free string if dynamically allocated
        free(tmp);
    }
}

/*int main(void)
{
    t_minishell *general = calloc(1, sizeof(t_minishell));
    if (!general)
        return 1;

    general->raw_line = strdup("ls &&wc ||grep ;\"hello\" << (outfile)||hi"); // copy to allow modification
    if (!general->raw_line)
    {
        free(general);
        return 1;
    }

    if (handle_lexer(general))
    {
        printf("Lexer tokens:\n");
        print_lexer(general->lexer);
    }

    // Clean up
    free_lexer(general->lexer);
    free(general->raw_line);
    free(general);

    return 0;
}
*/