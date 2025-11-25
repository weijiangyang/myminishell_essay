#include "../../include/minishell.h"
#include "../../libft/libft.h"

/**
 * @brief 为解析到的重定向 token 创建一个 t_redir 结构节点。
 *
 * 内存所有权说明：
 * - 本函数负责分配 t_redir 节点本体（calloc）。
 * - 并 strdup() 一份 content，使 filename 在 AST 中完全独立存在。
 * - 调用者（通常是 parser）只需把返回值挂入重定向链表。
 * - 后续由 free_redir_list() 统一释放节点及其 filename。
 *
 * heredoc_fd：
 * - 初始设为 -1（无效值）。真正执行 heredoc 时才会被赋予有效 FD。
 * - free_redir_list() 中会在 FD >= 0 时自动 close。
 *
 * @param type    token 类型（来自 lexer）
 * @param content redirect 后面的文件名字符串
 *
 * @return t_redir* 成功返回新节点；失败返回 NULL。
 */

t_redir *create_redir(tok_type type, char *content)
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
/**
 * @brief 创建一个参数节点（t_cmd），用于构建命令的 argv 链表。
 *
 * 内存所有权说明：
 * - 分配一个 t_cmd 结构（malloc）。
 * - 使用 strdup() 存储参数字符串，保证与 lexer token 完全独立。
 * - 释放：由 free_argv_list() 负责释放 arg 和节点自身。
 *
 * 用途：
 * - parser 在解析普通命令时，每遇到一个 TOK_WORD，就创建一个 argv 节点，
 *   并挂入 argv 链表，最后统一构造成 char **argv。
 *
 * @param str  原始参数字符串（通常来自 token->str）
 *
 * @return t_cmd* 成功返回新节点；失败返回 NULL。
 */
t_cmd *create_argv(char *str)
{
    char *dup;

    if (!str)
        return NULL;
    dup = ft_strdup(str);
    if (!dup)
        return NULL;
    return ft_lstnew(dup);
}

/**
 * @brief 将一个新的重定向节点 (t_redir) 追加到重定向链表 lst 的末尾。
 *
 * 用途：
 *   - parser 在解析命令时，每遇到一个重定向（<, >, >>, <<），
 *     就创建一个 t_redir 节点（create_redir），然后追加到链表末尾。
 *
 * 内存所有权：
 *   - 本函数不分配内存，只负责链接已经分配好的 new_node。
 *   - 重定向链表最终由 free_redir_list() 统一释放，包括 filename 和 heredoc_fd。
 *
 * @param lst       指向链表头指针的地址（允许修改头指针）
 * @param new_node  已分配好的 t_redir 节点
 *
 * @return t_redir* 返回追加的节点；失败返回 NULL。
 */
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

static int handle_heredoc(t_redir *redir)
{
    int pipefd[2];
    char *line = NULL;
    size_t len = 0;

    if (pipe(pipefd) < 0)
        return (-1);
    while (1)
    {
        ssize_t nread;
        write(1, "heredoc< ", 9);
        nread = getline(&line, &len, stdin);
        if (nread < 0)
            break;
        ssize_t cmp_len = nread;
        if (cmp_len > 0 && line[cmp_len - 1] == '\n')
            cmp_len--;
        if ((ssize_t)ft_strlen(redir->filename) == cmp_len
            && strncmp(line, redir->filename, cmp_len) == 0)
            break;
        write(pipefd[1], line, nread);
    }
    free(line);
    close(pipefd[1]);
    redir->heredoc_fd = pipefd[0];
    return (0);
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

            /* heredoc safer loop */
            if (op->tokentype == TOK_HEREDOC)
                handle_heredoc(redir);
            redirlst_add_back(&redir, new_redir);
        }
        else if (pt->tokentype == TOK_WORD)
        {
            t_lexer *tok = consume_token(cur);
            t_cmd *new_argv = create_argv(tok->str);
            ft_lstadd_back(&argv_cmd, new_argv);
        }
        else
            break;
    }
    int size = ft_lstsize(argv_cmd);
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
        argvs[i++] = tmp->content;
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