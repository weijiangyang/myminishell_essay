/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_ast_tokens.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:28:05 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:28:18 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
 *
 * 本函数设计为绝对安全，不会发生：
 *      - double free
 *      - double close
 *      - use-after-free
 * */
void free_redir_list(t_redir *r)
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
void free_ast_partial(ast *node)
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
 * free_ast
 * ----------------
 * 目的：
 *   递归释放 AST（抽象语法树）节点及其所有子节点的动态内存。
 *   根据节点类型分别处理：
 *     - NODE_CMD：释放命令节点的 argv 和重定向字段
 *     - NODE_PIPE：递归释放左右子树
 *     - NODE_SUBSHELL：递归释放子 shell 的内部 AST
 *
 * 参数：
 *   - node : 指向要释放的 AST 节点指针（允许为 NULL）
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 注意：
 *   - 最后会 free(node) 自身。
 */

void free_ast(ast *node)
{
    if (!node)
        return;
    if (node->type == NODE_CMD)
    {
        free_ast_partial(node);
        return ;
    } 
    else if (node->type == NODE_PIPE)
    {
        free_ast(node->left);
        free_ast(node->right);
    }
    else if (node->type == NODE_SUBSHELL)
        free_ast(node->sub);
    free(node);
}


/**
 * free_tokens
 * ----------------
 * 目的：
 *   释放词法分析阶段生成的 token 链表，
 *   包括每个 token 的字符串字段和节点自身。
 *
 * 参数：
 *   - tok : 指向 token 链表头部的指针（允许为 NULL）
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 行为说明：
 *   - 逐个遍历链表节点
 *   - 先释放 token->str（如果存在）
 *   - 再释放 token 节点本身
 *   - 最终释放整个链表
 */
void free_tokens(t_lexer *tok)
{
    while (tok)
    {
        t_lexer *nx = tok->next;

        if (tok->str)
            free(tok->str);

        free(tok);
        tok = nx;
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

        /* 释放 strdup 得到的字符串 */
        free(a->content);

        /* 释放链表节点本体 */
        free(a);

        a = next;
    }
}


