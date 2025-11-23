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

        if (r->type == HEREDOC && r->heredoc_fd >= 0)
            close(r->heredoc_fd);

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
    t_redir *new_node = ft_calloc(1, sizeof(t_redir));
    if (!new_node)
        return NULL;

    new_node->filename = ft_strdup(content);
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
            t_lexer *op = consume_token(cur);
            t_lexer *filetok = consume_token(cur);
            if (!op || !filetok || filetok->tokentype != TOK_WORD)
            {
                free_ast_partial(node);
                return NULL;
            }

            t_redir *new_redir = create_redir(op->tokentype, filetok->str);
            if (!new_redir)
            {
                free_ast_partial(node);
                return NULL;
            }

            if (op->tokentype == TOK_HEREDOC)
            {
                int pipefd[2];
                if (pipe(pipefd) < 0)
                {
                    perror("pipe");
                    free(new_redir);
                    free_ast_partial(node);
                    return NULL;
                }
                while (1)
                {
                    char *line = NULL;
                    size_t len = 0;

                    write(1, "heredoc< ", 9);

                    ssize_t nread = getline(&line, &len, stdin);
                    if (nread <= 0)
                    {
                        free(line); // 修复：getline 失败时可能需要 free
                        break;
                    }
                    // 确保行末有 \n（getline 通常有，但重定向或 EOF 可能没有）
                    int has_newline = (line[nread - 1] == '\n');

                    // 暂时去掉末尾的换行便于比较
                    if (has_newline)
                        line[nread - 1] = '\0';

                    // 与 delimiter 比较
                    if (strcmp(line, new_redir->filename) == 0)
                    {
                        free(line);
                        break;
                    }
                    // 恢复换行
                    if (has_newline)
                        line[nread - 1] = '\n';

                    // 写入 pipe：必须写入实际读取的 nread 字节，而不是 len
                    write(pipefd[1], line, nread);

                    free(line);
                }

                close(pipefd[1]);                  // 关闭写端
                new_redir->heredoc_fd = pipefd[0]; // 读端留给 exec 阶段使用
            }
            redirlst_add_back(&redir, new_redir);
        }
        else if (pt->tokentype == TOK_WORD)
        {
            t_lexer *tok = consume_token(cur);
            t_cmd *new_argv = create_argv(tok->str);
            cmdlst_add_back(&argv_cmd, new_argv);
        }
        else
            break;
    }
    int size = cmdlst_size(argv_cmd);
    char **argvs = malloc((size + 1) * sizeof(char *));
    if (!argvs)
    {
        free_redir_list(redir);
        free_argv_list(argv_cmd);
        free(node);
        return NULL;
    }
    int i = 0;
    t_cmd *tmp = argv_cmd;
    while (tmp && i < size)
    {
        argvs[i++] = tmp->arg;
        tmp = tmp->next;
    }
    argvs[i] = NULL;
    tmp = argv_cmd;
    while (tmp)
    {
        t_cmd *next = tmp->next;
        free(tmp);
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