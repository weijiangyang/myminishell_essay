/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_redir_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 16:17:59 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/25 16:18:01 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * create_redir
 * ------------------------------------------------------------
 * 目的：
 *   为解析阶段创建一个新的重定向节点（t_redir），并根据
 *   token 类型设置其重定向种类（输入、输出、追加、heredoc）。
 *   同时会复制文件名（或 delimiter）字符串，以保证内存独立性。
 *
 * 参数：
 *   @type    - 来自词法分析的 token 类型（tok_type）
 *              TOK_REDIR_IN, TOK_REDIR_OUT, TOK_APPEND, TOK_HEREDOC
 *
 *   @content - 重定向后跟随的目标文件名或 heredoc 的 delimiter。
 *              该字符串由 lexer 提供，这里会 strdup() 创建副本。
 *
 * 返回值：
 *   成功：返回新创建且初始化完毕的 t_redir 指针
 *   失败：返回 NULL（内存分配失败）
 *
 * 逻辑说明：
 *   1. 分配一个 t_redir 节点并初始化为 0。
 *   2. 使用 strdup() 复制 content，确保 redir 节点拥有自己的内存。
 *   3. 根据 token 类型设置节点的重定向类型：
 *         - TOK_REDIR_IN   -> `<`
 *         - TOK_REDIR_OUT  -> `>`
 *         - TOK_APPEND     -> `>>`
 *         - TOK_HEREDOC    -> `<<`
 *   4. heredoc_fd 初始化为 -1，表示暂未创建管道。
 *   5. 返回配置完成的节点。
 */
static t_redir *create_redir(t_minishell *minishell, tok_type type, char *content)
{
    t_redir *new_node = ft_calloc(1, sizeof(t_redir));
    if (!new_node)
        return NULL;
    char *new_content = ft_strdup(content);
    if (!new_content)
    {
        free(new_node);
        return NULL;
    }
    new_node->filename = ft_strdup(expander_str(minishell, new_content));
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
 * redirlst_add_back
 * ------------------------------------------------------------
 * 目的：
 *   将一个新的重定向节点（t_redir）追加到重定向链表末尾。
 *   该链表保存所有与当前命令相关的重定向（<, >, >>, <<）。
 *
 * 参数：
 *   @lst       - 指向重定向链表头指针的地址（t_redir**）。
 *                如果链表为空，本函数会将 new_node 设为链表头。
 *
 *   @new_node  - 已分配并初始化好的 t_redir 节点。
 *                其 next 字段应由调用者保证为 NULL。
 *
 * 返回值：
 *   无（void）。
 *   若 lst 或 new_node 为 NULL，本函数将不进行任何操作。
 *
 * 逻辑：
 *   1. 检查参数有效性。
 *   2. 若链表为空，则直接将 new_node 设为头节点。
 *   3. 若链表非空，则遍历至尾节点，并将 new_node 挂在末尾。
 */
static void redirlst_add_back(t_redir **lst, t_redir *new_node)
{
    t_redir *tmp;

    if (!lst || !new_node)
        return;
    if (*lst == NULL)
    {
        *lst = new_node;
        return;
    }
    tmp = *lst;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new_node;
}

/**
 * build_redir
 * ------------------------------------------------------------
 * 目的：
 *   从当前 lexer 位置解析一个重定向操作（<, >, >>, <<），
 *   构建对应的 t_redir 节点并追加到重定向链表中。
 *
 * 参数：
 *   @cur   - 指向当前 lexer 指针的地址（t_lexer**）。
 *            本函数会从 token 流中消费两个 token：
 *            1. 重定向操作符（<, >, >>, <<）
 *            2. 后面的文件名（TOK_WORD）
 *
 *   @node  - 当前正在构建的 AST 节点。
 *            若解析失败，会调用 free_ast_partial(node) 释放资源。
 *
 *   @redir - 已存在的重定向链表头。
 *            若为 NULL，本函数会创建新的头节点。
 *
 * 返回值：
 *   成功：返回更新后的重定向链表头（可能与原 redir 相同或变为 new）。
 *   失败：返回 NULL，并释放 node 的部分资源。
 *
 * 逻辑：
 *   1. 从 token 流中取出重定向符号与下一 token（文件名）。
 *   2. 若格式错误或 token 类型不正确，则释放 AST 并返回 NULL。
 *   3. 根据 token 类型创建对应的 t_redir 节点（create_redir）。
 *   4. 如果是 heredoc (<<)，调用 handle_heredoc() 处理内容。
 *   5. 将新节点追加到 redir 链表末尾。
 *   6. 返回更新后的 redir 链表头。
 */
t_redir *build_redir(t_lexer **cur, ast *node, t_redir *redir, t_minishell *minishell)
{
    t_lexer *op = consume_token(cur);
    t_lexer *filetok = consume_token(cur);
    t_redir *new_redir;

    if (!op || !filetok || filetok->tokentype != TOK_WORD)
    {
        free_ast_partial(node);
        return NULL;
    }
    new_redir = create_redir(minishell, op->tokentype, filetok->str);
    if (!new_redir)
    {
        free_ast_partial(node);
        return NULL;
    }
    if (op->tokentype == TOK_HEREDOC)
    {
        if (handle_heredoc(new_redir, minishell) == -1)
        {
            redirlst_add_back(&redir, new_redir);
            return NULL; // 返回 NULL 上层可检测停止命令执行
        }
    }
    redirlst_add_back(&redir, new_redir);
    return redir;
}
