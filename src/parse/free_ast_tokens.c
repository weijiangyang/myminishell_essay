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
 * free_ast_cmd
 * ----------------
 * 目的：
 *   释放普通命令节点 (NODE_CMD) 中所有动态分配的内存，
 *   包括 argv 数组及各类重定向字符串。
 *
 * 参数：
 *   - node : 指向要释放的命令 AST 节点（不释放 node 本身）
 *
 * 返回值：
 *   - 无返回值（void）
 */
static void free_ast_cmd(ast *node)
{
    int i;

    i = 0;
    if (node->argv)
    {  
        while (node->argv[i])
        {
            free(node->argv[i]);
            i++;
        }     
        free(node->argv);
    } 
    free(node->redir_in);  
    free(node->redir_out);   
    free(node->redir_append); 
    free(node->heredoc_delim);
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
        free_ast_cmd(node);
    else if (node->type == NODE_PIPE)
    {
        free_ast(node->left);
        free_ast(node->right);
    }
    else if (node->type == NODE_SUBSHELL)
        free_ast(node);
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
