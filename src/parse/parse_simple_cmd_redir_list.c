#include "../../include/minishell.h"
#include "../../libft/libft.h"

/**
 * @struct s_cmd
 * @brief  单向链表节点，用于在解析阶段暂存命令参数（argv）。
 *
 * 该结构体只在“解析阶段”使用，用于临时保存每一个解析到的
 * 单词（token），最终会被转换为 AST 节点中的 node->argv（char **）。
 *
 * 生命周期与所有权（非常关键）：
 * --------------------------------------------------------------
 * 1) 在解析时，每读到一个 TOK_WORD，就会创建一个 t_cmd 节点：
 *        new->arg = strdup(token->str);
 *
 * 2) 解析完成后，会将整个 t_cmd 链表复制到 node->argv：
 *        node->argv[i] = strdup(tmp->arg);
 *
 * 3) 复制完成后，这个链表中的字符串(new->arg) 及链表节点本身
 *    会由 free_argv_list() 统一释放。
 *
 * 重要：最终的内存所有者是 AST 中的 node->argv（char **）。
 *      t_cmd 链表只是构造阶段的临时容器。
 *
 * 这样设计的好处：
 *  - 避免 node->argv 与临时链表共享同一块字符串（从而避免 double-free）。
 *  - 解析时能方便按顺序收集参数。
 *  - 错误恢复逻辑（free_ast_partial）更简单明确。
 *
 * @field arg   strdup 得到的单个参数字符串（临时所有者）
 * @field next  指向下一个 t_cmd 节点（单向链表）
 */
typedef struct s_cmd
{
    char *arg;
    struct s_cmd *next;
} t_cmd;

/**
 * @brief 释放整个重定向链表（t_redir）。
 *
 * 内存/资源所有权规则：
 * ------------------------------------------------------
 * 1) r->filename
 *      - 在 create_redir() 中 strdup() 得到
 *      - 因此由本函数负责 free()。
 *
 * 2) HEREDOC 类型：
 *      - r->heredoc_fd 在 parse 阶段用 pipe() 生成
 *      - exec 阶段会读取该 fd
 *      - 解析出错时或 AST 被释放时，必须由本函数 close(fd)
 *      - 确保不重复 close：只有当 fd >= 0 时才 close。
 *
 * 3) 整个 t_redir 节点由 ft_calloc/malloc 创建
 *      → 本函数必须 free(r)
 *
 * 本函数设计为绝对安全，不会发生：
 *      - double free
 *      - double close
 *      - use-after-free
 */
static void free_redir_list(t_redir *r)
{
    t_redir *next;

    while (r)
    {
        next = r->next;

        /* 关闭 heredoc 的 read-end（若有效） */
        if (r->type == HEREDOC && r->heredoc_fd >= 0)
        {
            close(r->heredoc_fd);
            r->heredoc_fd = -1;
        }

        /* 释放 strdup() 的文件名 */
        free(r->filename);

        /* 释放节点本体 */
        free(r);

        r = next;
    }
}
/**
 * @brief 释放解析阶段的临时 argv 链表（t_cmd）。
 *
 * 所有权模型（非常重要）：
 * --------------------------------------------------------------
 * - t_cmd->arg 是在 create_argv() 中通过 strdup 创建的。
 * - 该链表 *只是解析阶段的临时容器*：
 *       TOK_WORD -> create_argv(strdup)
 *
 * - 在解析完成后，这个链表会被拷贝到 AST 的 node->argv：
 *       node->argv[i] = strdup(a->arg);
 *
 * - 所以：
 *       t_cmd->arg 和 node->argv[i] 指向 *不同* 的内存块
 *       （各自是一次 strdup）
 *
 * - 这使得我们可以安全地：
 *       free(a->arg);          // 释放临时链表的 strdup
 *       free(a);               // 释放链表节点本体
 *
 * - 而不会影响 AST 的 node->argv（最终生存的 argv）。
 *
 * 该函数只释放链表，不影响 AST。
 */
static void free_argv_list(t_cmd *a)
{
    t_cmd *next;

    while (a)
    {
        next = a->next;

        /* 释放 strdup 得到的字符串 */
        free(a->arg);

        /* 释放链表节点本体 */
        free(a);

        a = next;
    }
}
/**
 * @brief 在解析出错时释放尚未完整连接的 AST 节点。
 *
 * 所有权说明：
 * --------------------------------------------------------------
 * - node->argv 是最终执行要用的 argv（char **），它里面的每个字符串
 *   都是 strdup 得来的，与解析阶段的 t_cmd->arg 没有共享内存。
 *
 * - t_cmd 链表（解析阶段临时 argv）已在其它地方通过 free_argv_list()
 *   释放，因此不会造成 double-free。
 *
 * - 因此这里必须逐个 free node->argv[i]，然后再 free node->argv 本体。
 */
static void free_ast_partial(ast *node)
{
    int i;

    if (!node)
        return;

    /* 释放重定向链表 */
    if (node->redir)
        free_redir_list(node->redir);

    /* 释放 argv 数组（char **） */
    if (node->argv)
    {
        /* 逐个释放 strdup 得到的字符串 */
        for (i = 0; node->argv[i]; i++)
            free(node->argv[i]);

        /* 释放数组本体 */
        free(node->argv);
    }

    /* 释放 AST 节点本体 */
    free(node);
}

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
    t_cmd *new_argv;

    new_argv = (t_cmd *)malloc(sizeof(t_cmd));
    if (!new_argv)
        return (NULL);
    new_argv->arg = ft_strdup(str);
    new_argv->next = NULL;
    return (new_argv);
}

/**
 * @brief 将一个新的参数节点 (t_cmd) 追加到参数链表 lst 的末尾。
 *
 * 用途：
 *   构建命令的 argv 链表。parser 每次遇到一个 TOK_WORD，
 *   就创建一个 t_cmd 节点（create_argv），然后用此函数追加到链表末尾。
 *
 * 内存所有权：
 *   - 本函数不分配内存，只链接你已经创建好的 new_node。
 *   - lst 的最终释放由 free_argv_list() 管理。
 *
 * @param lst       指向链表头指针的地址（允许修改头指针）
 * @param new_node  已分配好的 t_cmd 节点
 *
 * @return t_cmd*   返回追加的节点指针；失败返回 NULL。
 */
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
/**
 * @brief 计算参数链表 t_cmd 的节点数量。
 *
 * 用途：
 *   - 在构造 AST 节点的 argv 数组（char **argv）时，需要知道链表长度，
 *     以便分配合适大小的二维数组。
 *
 * 内存/所有权说明：
 *   - 仅遍历链表，不修改链表结构。
 *   - 不分配或释放任何内存。
 *
 * @param lst  t_cmd 链表头
 * @return int 返回链表节点数量
 */
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

            /* heredoc safer loop */
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
                    ssize_t nread;

                    write(1, "heredoc< ", 9);
                    nread = getline(&line, &len, stdin);
                    if (nread < 0)
                    {
                        free(line);
                        break;
                    }

                    /* 比较时不包含末尾换行 */
                    ssize_t cmp_len = nread;
                    if (cmp_len > 0 && line[cmp_len - 1] == '\n')
                        cmp_len--;

                    if ((ssize_t)ft_strlen(new_redir->filename) == cmp_len && strncmp(line, new_redir->filename, cmp_len) == 0)
                    {
                        free(line);
                        break;
                    }

                    /* 写入全部读取到的字节（包含换行符，如果有） */
                    ssize_t written = 0;
                    while (written < nread)
                    {
                        ssize_t w = write(pipefd[1], line + written, nread - written);
                        if (w < 0)
                        {
                            perror("write");
                            free(line);
                            close(pipefd[1]);
                            close(pipefd[0]);
                            free(new_redir->filename);
                            free(new_redir);
                            free_redir_list(redir);
                            free_ast_partial(node);
                            return NULL;
                        }
                        written += w;
                    }
                    free(line);
                }
                close(pipefd[1]);
                new_redir->heredoc_fd = pipefd[0];
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