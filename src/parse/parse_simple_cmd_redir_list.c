#include "../../include/minishell.h"

/**
 * create_argv
 * ------------------------------------------------------------
 * 目的：
 *   为命令参数创建一个 t_cmd 节点，并将参数字符串存储在节点中。
 *   该节点通常用于构建命令的 argv 链表。
 *
 * 参数：
 *   @str — 原始参数字符串（通常来自 lexer token）。
 *
 * 返回值：
 *   成功：返回新创建的 t_cmd 节点（内含 strdup 拷贝的字符串）。
 *   失败：返回 NULL（str 为 NULL 或内存分配失败）。
 *
 * 逻辑：
 *   1. 检查输入 str 是否为 NULL。
 *   2. 使用 ft_strdup 拷贝字符串，保证内存独立。
 *   3. 使用 ft_lstnew 创建新的 t_cmd 节点，将 strdup 的字符串放入 content。
 *   4. 返回新节点，供命令解析链表使用。
 *
 * 特性：
 *   - 避免对原始字符串进行直接引用，保证内存安全。
 *   - 节点释放由 free_argv_list 统一管理。
 */
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

/**
 * build_argvs
 * ------------------------------------------------------------
 * 目的：
 *   将 t_cmd 链表中的参数构建成 char **argv 数组，便于 AST 节点使用。
 *   同时释放临时 t_cmd 链表节点，但保留 argv 数组的字符串内容。
 *
 * 参数：
 *   @argv_cmd — t_cmd 链表头，包含命令参数（每个节点的 content 为 strdup 的字符串）。
 *   @redir    — 当前命令的重定向链表头（用于失败时释放）。
 *   @node     — 当前 AST 节点（用于失败时释放）。
 *
 * 返回值：
 *   成功：返回 char **argv（以 NULL 结尾）。
 *   失败：返回 NULL，并释放 argv_cmd、redir 和 node。
 *
 * 逻辑：
 *   1. 计算 argv_cmd 链表长度 size。
 *   2. 分配 char **argvs，大小为 size + 1。
 *      - 若分配失败，调用 free_redir_list、free_argv_list 和 free(node) 释放资源。
 *   3. 遍历 argv_cmd 链表，将每个 content 指针存入 argvs 数组。
 *   4. 末尾添加 NULL 作为数组结束标记。
 *   5. 调用 free_t_cmd_node 释放链表节点本体（不释放 content）。
 *   6. 返回构建好的 argvs 数组。
 *
 * 特性：
 *   - 确保 argv 数组与原 t_cmd 链表的字符串独立，内存安全。
 *   - 避免 double-free，链表节点和字符串释放职责分开。
 */
static char **build_argvs(t_cmd *argv_cmd, t_redir *redir, ast *node)
{
    int size;
    char **argvs;
    int i;
    t_cmd *tmp;

    if (argv_cmd == NULL)
        return (NULL);
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

/**
 * parse_normal_cmd_redir_list
 * ------------------------------------------------------------
 * 目的：
 *   解析普通命令及其重定向列表，构建 AST 节点。
 *   - 处理命令参数 (TOK_WORD) → 构建 t_cmd 链表 → 转为 char **argv
 *   - 处理重定向 (>, <, >>, <<) → 构建 t_redir 链表
 *
 * 参数：
 *   @cur  — 指向当前 token 的指针（指针的指针，用于消费 token）
 *   @node — 已分配的 AST 节点，函数在其上填充 type、argv 和 redir
 *
 * 返回值：
 *   - 成功：返回填充好的 AST 节点
 *   - 失败：返回 NULL，并在内部释放 node、argv_cmd 链表及部分 redir
 *
 * 逻辑：
 *   1. 初始化重定向链表和 argv 链表为空。
 *   2. 设置 AST 节点类型为 NODE_CMD。
 *   3. 遍历 token：
 *      a. 如果 token 是重定向符号 → 调用 build_redir 构建或追加到 redir 链表。
 *      b. 如果 token 是普通命令参数 (TOK_WORD) → 调用 create_argv 创建 t_cmd 节点并追加。
 *      c. 否则跳出循环。
 *   4. 将最终重定向链表赋给 node->redir。
 *   5. 调用 build_argvs 将 t_cmd 链表转换成 char **argv，并赋给 node->argv。
 *   6. 返回 AST 节点。
 *
 * 内存安全：
 *   - build_redir 内部处理 heredoc、文件描述符及失败释放。
 *   - build_argvs 内部分配 argv 数组并释放 t_cmd 链表节点。
 *   - 避免 double free，确保 AST 节点在失败时安全释放。
 */
static ast *parse_normal_cmd_redir_list(t_lexer **cur, ast *node, t_minishell *minishell)
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
            redir = build_redir(cur, node, redir, minishell);
            if (!redir)
                return NULL; // ❗ 立刻终止解析
        }

        else if (pt->tokentype == TOK_WORD)
            ft_lstadd_back(&argv_cmd, create_argv(consume_token(cur)->str));
        else
            break;
    }
    if (!argv_cmd && !redir)
    {
        if (minishell->last_exit_status != 130)
            minishell->last_exit_status = 2;
        return NULL;
    }
    node->redir = redir;
    node->argv = build_argvs(argv_cmd, redir, node);
    return node;
}

/**
 * parse_simple_cmd_redir_list
 * ------------------------------------------------------------
 * 目的：
 *   解析一个简单命令（可能带重定向或子 shell），构建对应的 AST 节点。
 *   - 如果是子 shell（以 '(' 开始） → 调用 parse_subshell
 *   - 否则 → 调用 parse_normal_cmd_redir_list 解析普通命令及重定向
 *
 * 参数：
 *   @cur — 指向当前 token 的指针（指针的指针，用于消费 token）
 *
 * 返回值：
 *   - 成功：返回构建好的 AST 节点
 *   - 失败：返回 NULL（分配 node 或解析失败）
 *
 * 逻辑：
 *   1. 查看当前 token。
 *   2. 分配 AST 节点 node。
 *      - 分配失败直接返回 NULL。
 *   3. 判断当前 token：
 *      - 如果是 '(' → 调用 parse_subshell 构建子 shell AST。
 *      - 否则 → 调用 parse_normal_cmd_redir_list 构建普通命令 AST。
 */
ast *parse_simple_cmd_redir_list(t_lexer **cur, t_minishell *minishell)
{
    ast *node;
    t_lexer *pt;

    pt = peek_token(cur);
    node = ft_calloc(1, sizeof(ast));
    if (!node)
        return (NULL);
    if (pt && pt->tokentype == TOK_LPAREN)
        return (parse_subshell(cur, node, minishell));
    return (parse_normal_cmd_redir_list(cur, node, minishell));
}