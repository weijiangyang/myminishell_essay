#include "../../include/minishell.h"
#include "../../libft/libft.h"

static t_redir *create_redir(tok_type type, char *content)
{
    t_redir *new_node = ft_calloc(1, sizeof(t_redir));
    if (!new_node)
        return NULL;

    new_node->filename = ft_strdup(content);
    if (!new_node->filename)
    {
        free(new_node);
        return NULL;
    }
    new_node->next = NULL;
    new_node->heredoc_fd = -1; // 明确无效值

    if (type == TOK_REDIR_IN)
        new_node->type = REDIR_INPUT;
    else if (type == TOK_REDIR_OUT)
        new_node->type = REDIR_OUTPUT;
    else if (type == TOK_APPEND)
        new_node->type = REDIR_APPEND;
    else if (type == TOK_HEREDOC)
        new_node->type = HEREDOC;
    return new_node;
}

static t_redir *redirlst_add_back(t_redir **lst, t_redir *new_node)
{
    t_redir *tmp;

    if (!lst || !new_node)
        return NULL;
    if (*lst == NULL)
    {
        *lst = new_node;
        return new_node;
    }
    tmp = *lst;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new_node;
    return new_node;
}

t_redir *build_redir(t_lexer **cur, ast *node, t_redir *redir)
{
    t_lexer *op;
    t_lexer *filetok;
    t_redir *new_redir;

    op = consume_token(cur);
    filetok = consume_token(cur);
    if (!op || !filetok || filetok->tokentype != TOK_WORD)
    {
        free_ast_partial(node);
        return NULL;
    }
    new_redir = create_redir(op->tokentype, filetok->str);
    if (!new_redir)
    {
        free_ast_partial(node);
        return NULL;
    }
    if (op->tokentype == TOK_HEREDOC)
        handle_heredoc(new_redir);
    redirlst_add_back(&redir, new_redir);
    return redir;
}