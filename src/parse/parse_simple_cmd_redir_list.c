#include "../../include/minishell.h"
#include "../../libft/libft.h"

typedef struct s_cmd
{
    char *arg;
    struct s_cmd *next;
} t_cmd;

static void free_redir_list(t_redir *r)
{
    t_redir *next;
    while (r)
    {
        next = r->next;
        if (r->filename)
            free(r->filename);
        free(r);
        r = next;
    }
}

static void free_argv_list(t_cmd *a)
{
    t_cmd *next;
    while (a)
    {
        next = a->next;
        if (a->arg)
            free(a->arg);
        free(a);
        a = next;
    }
}
/* 简单 free_ast 用于出错时释放 node + 内部链表/argvs（如果有）*/
static void free_ast_partial(ast *node)
{
    if (!node)
        return;
    if (node->redir)
        free_redir_list(node->redir);
    if (node->argv)
    {
        /* node->argv 是 char **，里面的字符串已被 strdup 并已在 argv 链表中释放过，
           为避免 double-free 我们在构造 node->argv 前不会 free argv 链表。
           这里仅 free 二维数组指针本身（如果分配过）*/
        free(node->argv);
    }
    free(node);
}

t_redir *create_redir(tok_type type, char *content)
{
    t_redir *new_node;

    new_node = ft_calloc(sizeof(t_redir), 1);
    if (!new_node)
        return (NULL);
    new_node->filename = content;
    if (type == TOK_REDIR_IN)
        new_node->type = REDIR_INPUT;
    else if (type == TOK_REDIR_OUT)
        new_node->type = REDIR_OUTPUT;
    else if (type == TOK_APPEND)
        new_node->type = REDIR_APPEND;
    else if (type == TOK_HEREDOC)
        new_node->type = HEREDOC;

    new_node->next = NULL;
    return (new_node);
}

t_cmd *create_argv(char *str)
{
    t_cmd *new_argv;

    new_argv = (t_cmd *)malloc(sizeof(t_cmd));
    if (!new_argv)
        return (NULL);
    new_argv->arg = ft_strdup(str);
    new_argv->next = NULL;
    return (new_argv);
}
// cmd list
t_cmd *cmdlst_add_back(t_cmd **lst, t_cmd *new_node)
{
    t_cmd *tmp;

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


static int cmdlst_size(t_cmd *lst)
{
    int n = 0;
    while (lst)
    {
        n++;
        lst = lst->next;
    }
    return n;
}

// redir list
t_redir *redirlst_add_back(t_redir **lst, t_redir *new_node)
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


static ast *parse_normal_cmd_redir_list(t_lexer **cur, ast *node)
{
    t_lexer *pt;
    t_redir *redir = NULL;
    t_cmd *argv_cmd = NULL;

    if (!cur || !node)
        return NULL;

    node->type = NODE_CMD;

    while ((pt = peek_token(cur)) != NULL)
    {
        if (is_redir_token(pt))
        {
            /* 直接 consume 重定向符号并拿到 op */
            t_lexer *op = consume_token(cur);
            if (!op)
            {
                free_ast_partial(node);
                fprintf(stderr, "Internal error: consume_token returned NULL for redir op\n");
                return NULL;
            }

            /* 直接 consume filename（比 peek+consume 更稳健）*/
            t_lexer *filetok = consume_token(cur);
            if (!filetok || filetok->tokentype != TOK_WORD)
            {
                free_ast_partial(node);
                fprintf(stderr, "Syntax error: Missing filename after redirection\n");
                return NULL;
            }

            /* create_redir 会 strdup filename（你的实现），要检查返回值 */
            t_redir *new_redir = create_redir(op->tokentype, filetok->str);
            if (!new_redir)
            {
                free_ast_partial(node);
                fprintf(stderr, "Memory error: create_redir failed\n");
                return NULL;
            }

            redirlst_add_back(&redir, new_redir);
        }
        else if (pt->tokentype == TOK_WORD)
        {
            /* consume word 并使用返回的 token */
            t_lexer *tok = consume_token(cur);
            if (!tok)
            {
                free_redir_list(redir);
                free_ast_partial(node);
                fprintf(stderr, "Internal error: consume_token returned NULL for word\n");
                return NULL;
            }

            t_cmd *new_argv = create_argv(tok->str);
            if (!new_argv)
            {
                free_redir_list(redir);
                free_ast_partial(node);
                fprintf(stderr, "Memory error: create_argv failed\n");
                return NULL;
            }

            cmdlst_add_back(&argv_cmd, new_argv);
        }
        else
        {
            /* 遇到其他 token（如管道、分号等）时停止解析当前 simple command */
            break;
        }
    }

    /* 构建 argv 二维数组：把链表中的字符串指针搬到 argv 数组，然后释放链表节点结构（但不 free 字符串） */
    int size = cmdlst_size(argv_cmd);
    char **argvs = malloc((size + 1) * sizeof(char *));
    if (!argvs)
    {
        free_redir_list(redir);
        free_argv_list(argv_cmd); // 此函数会 free strdup 的字符串，若你计划保留字符串请调整
        free(node);
        fprintf(stderr, "Memory error: malloc argv array failed\n");
        return NULL;
    }

    int i = 0;
    t_cmd *tmp = argv_cmd;
    while (tmp && i < size)
    {
        argvs[i++] = tmp->arg; /* 迁移字符串所有权到 argv 数组 */
        tmp = tmp->next;
    }
    argvs[i] = NULL;

    /* 释放 argv 链表节点，但不要 free 字符串（因为已在 argvs 中） */
    tmp = argv_cmd;
    while (tmp)
    {
        t_cmd *next = tmp->next;
        free(tmp); /* 仅释放节点结构 */
        tmp = next;
    }

    node->redir = redir;
    node->argv = argvs;

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