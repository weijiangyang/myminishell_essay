/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ast_by_type.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:30:54 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:30:57 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
** print_ast_cmd
** ----------------
** 打印普通命令节点 (NODE_CMD) 的信息，包括：
**   - 命令名和参数 (argv)
**   - 输入重定向 (<)
**   - 输出重定向 (>, >>)
**   - heredoc (<<)
**
** 参数：
**   - node : 指向 AST 命令节点
*/
void print_ast_cmd(ast *node)
{
    size_t i = 0;

    printf("CMD"); // 打印节点类型

    // 打印命令及其参数
    while (node->argv && node->argv[i])
    {
        printf(" \"%s\"", node->argv[i]);
        i++;
    }
    // 打印可能存在的重定向
    if (node->redir_in)
        printf(" < %s", node->redir_in);
    if (node->redir_out)
        printf(" > %s", node->redir_out);
    if (node->redir_append)
        printf(" >> %s", node->redir_append);
    if (node->heredoc_delim)
        printf(" << %s", node->heredoc_delim);
    if (node->is_background)
        printf("&");

    printf("\n"); // 换行
}

/*
** print_ast_pipe
** ----------------
** 打印管道节点 (NODE_PIPE)。
** 递归打印左子树和右子树，并增加缩进表示层级。
**
** 参数：
**   - node  : 当前 PIPE 节点
**   - depth : 当前树深度，用于缩进显示
*/
void print_ast_pipe(ast *node, int depth)
{
    printf("PIPE\n");
    print_ast(node->left, depth + 1);  // 打印左子树
    print_ast(node->right, depth + 1); // 打印右子树
}

/*
** print_ast_and
** ----------------
** 打印逻辑与节点 (NODE_AND)。
** 递归打印左右子树。
**
** 参数：
**   - node  : 当前 AND 节点
**   - depth : 当前树深度
*/
void print_ast_and(ast *node, int depth)
{
    printf("AND\n");
    print_ast(node->left, depth + 1);
    print_ast(node->right, depth + 1);
}

/*
** print_ast_or
** ----------------
** 打印逻辑或节点 (NODE_OR)。
** 递归打印左右子树。
**
** 参数：
**   - node  : 当前 OR 节点
**   - depth : 当前树深度
*/
void print_ast_or(ast *node, int depth)
{
    printf("OR\n");
    print_ast(node->left, depth + 1);
    print_ast(node->right, depth + 1);
}

/*
** print_ast_subshell
** ----------------
** 打印子 shell 节点 (NODE_SUBSHELL)。
** 递归打印子 shell 内部的 AST。
**
** 参数：
**   - node  : 当前 SUBSHELL 节点
**   - depth : 当前树深度
*/
void print_ast_subshell(ast *node, int depth)
{
    printf("SUBSHELL\n");
    print_ast(node->sub, depth + 1);
}


