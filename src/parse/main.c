#include "../../include/minishell.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    char buf[1024];

    printf("Enter a shell command:\n");
    if (!fgets(buf, sizeof(buf), stdin))
    {
        return 0;
    }
    /* strip newline */
    buf[strcspn(buf, "\n")] = '\0';

    t_minishell *general = calloc(1, sizeof(t_minishell));
    general->raw_line = buf;
    if (!general)
        return 1;
    if (handle_lexer(general))
    {
        printf("Lexer tokens:\n");
        print_lexer(general->lexer);
    }
    if (!general->lexer)
    {
        fprintf(stderr, "tokenize failed\n");
        return 1;
    }

    /* parser now uses a cursor pointer */
    t_lexer *cursor = general->lexer;
    ast *root = parse_cmdline(&cursor);
    if (root)
    {
        printf("=== AST ===\n");
        print_ast(root, 0);
        exec_ast(root);
        free_ast(root);
    }
    else
    {
        fprintf(stderr, "Parsing failed.\n");
    }

    free_tokens(general->lexer);
    free(general);

    return 0;
}