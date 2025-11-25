#include "../../include/minishell.h"
#include "../../libft/libft.h"

static t_cmd *create_argv(char *str)
{
    char *dup;

    if (!str)
        return NULL;
    dup = ft_strdup(str);
    if (!dup)
        return NULL;
    return ft_lstnew(dup);
}
static char **build_argvs(t_cmd *argv_cmd, t_redir *redir, ast *node)
{
    int size;
    char **argvs;
    int i;
    t_cmd *tmp;

    size = ft_lstsize(argv_cmd);
    argvs = malloc((size + 1) * sizeof(char *));
    if (!argvs)
        return (free_redir_list(redir), free_argv_list(argv_cmd), free(node), NULL);
    i = 0;
    tmp = argv_cmd;
    while (tmp && i < size)
    {
        argvs[i++] = tmp->content;
        tmp = tmp->next;
    }
    argvs[i] = NULL;
    tmp = argv_cmd;
    free_t_cmd_node(tmp);
    return (argvs);
}

static ast *parse_normal_cmd_redir_list(t_lexer **cur, ast *node)
{
    t_lexer *pt;
    t_redir *redir;
    t_cmd *argv_cmd;

    redir = NULL;
    argv_cmd = NULL;

    if (!cur || !node)
        return NULL;
    node->type = NODE_CMD;
    while ((pt = peek_token(cur)) != NULL)
    {
        if (is_redir_token(pt))
            redir = build_redir(cur, node, redir);
        else if (pt->tokentype == TOK_WORD)
            ft_lstadd_back(&argv_cmd, create_argv(consume_token(cur)->str));
        else
            break;
    }
    node->redir = redir;
    node->argv = build_argvs(argv_cmd, redir, node);
    return node;
}

ast *parse_simple_cmd_redir_list(t_lexer **cur)
{
    ast *node;
    t_lexer *pt;

    pt = peek_token(cur);
    node = ft_calloc(1, sizeof(ast));
    if (!node)
        return (NULL);
    if (pt && pt->tokentype == TOK_LPAREN)
        return (parse_subshell(cur, node));
    return (parse_normal_cmd_redir_list(cur, node));
}