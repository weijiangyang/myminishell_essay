#include "../../include/minishell.h"
#include "../../libft/libft.h"

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
 * */
void free_redir_list(t_redir *r)
{
    t_redir *next;

    while (r)
    {
        next = r->next;
        if (r->type == HEREDOC && r->heredoc_fd >= 0)
        {
            close(r->heredoc_fd);
            r->heredoc_fd = -1;
        }
        free(r->filename);
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
void free_argv_list(t_cmd *a)
{
    t_cmd *next;

    while (a)
    {
        next = a->next;
        free(a->content);
        free(a);
        a = next;
    }
}

void free_t_cmd_node(t_cmd *argv_cmd)
{
    t_cmd *tmp;
    t_cmd *next;

    tmp = argv_cmd;
    while (tmp)
    {
        next = tmp->next;
        free(tmp);
        tmp = next;
    }
}