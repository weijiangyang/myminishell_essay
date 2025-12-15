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

/**
 * free_ast_partial
 * ------------------------------------------------------------
 * 目的：
 *   在解析失败或构建 AST 节点过程中提前退出时，
 *   安全释放已经分配的部分 AST 内容（redir、argv、节点本体）。
 *
 *   ⚠️ 与 free_ast 不同：
 *      - 本函数只释放“当前 node 自身”已分配的字段，
 *        不会递归释放子节点（left、right、sub）。
 *      - 专供 parser 在错误时调用。
 *
 * 参数：
 *   @node - 需要部分释放的 AST 节点。
 *           若为 NULL，则无操作。
 *
 * 返回值：
 *   无返回值（void）
 *
 * 逻辑：
 *   1. 若存在 node->redir，则调用 free_redir_list() 释放所有重定向节点。
 *   2. 若存在 node->argv：
 *        - 逐个释放 argv[i]（此前由 strdup 分配）
 *        - 释放 argv 数组本体
 *   3. 最后释放 AST 节点本体 node。
 */
void free_ast_partial(ast *node)
{
    int i;

    i = 0;
    if (!node)
        return;
    if (node->redir)
        free_redir_list(node->redir);
    if (node->argv)
    {
        while (node->argv[i])
        {
            free(node->argv[i]);
            i++;
        }
        free(node->argv);
    }
    free(node);
}

/**
 * free_ast
 * ------------------------------------------------------------
 * 目的：
 *   递归释放整棵 AST（抽象语法树），包含：
 *     - 命令节点（NODE_CMD）
 *     - 管道节点（NODE_PIPE）
 *     - 子 shell 节点（NODE_SUBSHELL）
 *
 *   根据节点类型决定释放策略：
 *     • NODE_CMD
 *         - 此类型节点具有 argv / redir 等资源
 *         - 交给 free_ast_partial() 完整释放
 *
 *     • NODE_PIPE
 *         - 左、右子树递归释放
 *
 *     • NODE_SUBSHELL
 *         - 释放其内部的子树 node->sub
 *
 * 参数：
 *   @node — 需要被递归释放的 AST 节点（允许为 NULL）。
 *
 * 返回值：
 *   无（void）
 *
 * 逻辑：
 *   1. 若 node 为 NULL，直接返回。
 *   2. 根据 node->type：
 *       - NODE_CMD：调用 free_ast_partial() 并 return（避免重复 free node）
 *       - NODE_PIPE：递归释放左右子树
 *       - NODE_SUBSHELL：递归释放子树
 *   3. 最后释放节点本体（非 NODE_CMD 情况）
 */
void free_ast(ast *node)
{
    if (!node)
        return;
    if (node->type == NODE_CMD)
    {
        free_ast_partial(node);
        return;
    }
    else if (node->type == NODE_PIPE)
    {
        if (node->left)
            free_ast(node->left);
        if (node->right)
            free_ast(node->right);
    }
    else if (node->type == NODE_SUBSHELL)
        free_ast(node->sub);
    free(node);
}

/**
 * free_tokens
 * ------------------------------------------------------------
 * 目的：
 *   释放词法分析阶段生成的 token 链表（t_lexer）。
 *   每个 token 节点都包含一个字符串字段（str），本函数负责完整释放：
 *      - token->str （通常由 strdup 分配）
 *      - token 节点本体
 *
 * 参数：
 *   @tok — token 链表的起始节点（可为 NULL）。
 *
 * 返回值：
 *   无（void）
 *
 * 逻辑：
 *   1. 逐个遍历链表节点。
 *   2. 对于每个节点：
 *        - 若 tok->str 不为空，则 free(tok->str)
 *        - free(token 节点本体)
 *   3. 移动到下一个节点，直到链表结束。
 *
 * 特性：
 *   - 无副作用，链表完全销毁。
 *   - 安全处理 NULL 指针。
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


