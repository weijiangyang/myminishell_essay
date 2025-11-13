#include "../../include/minishell.h"

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