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

/**
 * print_ast_cmd
 * ----------------
 * 目的：
 *   打印普通命令节点 (NODE_CMD) 的详细信息，
 *   包括命令名、参数以及各种重定向。
 *
 * 参数：
 *   - node : 指向 AST 命令节点
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 行为说明：
 *   1. 打印节点类型 "CMD"
 *   2. 遍历 argv 数组，依次打印命令名和参数
 *   3. 打印可能存在的重定向信息：
 *        - 输入重定向 (<)
 *        - 输出重定向 (>)
 *        - 追加重定向 (>>)
 *        - heredoc (<<)
 *   4. 换行结束
 */
const char *redir_type_to_string(t_redir_type type)
{
    switch (type) {
        case REDIR_INPUT:    return "REDIR_INPUT";
        case REDIR_OUTPUT:   return "REDIR_OUTPUT";
        case REDIR_APPEND:   return "REDIR_APPEND";
        case HEREDOC:  return "HEREDOC";
        // 如果还有别的 type，就继续写 case
        default:              return "UNKNOWN_REDIR_TYPE";
    }
}

void print_list_redir(t_redir *redir)
{
    while (redir)
    {
       const char *type_str = redir_type_to_string(redir->type);
        printf(" %s : %s ", type_str, redir->filename);
        
        redir = redir->next;
    }
}

void print_ast_cmd(ast *node)
{
    size_t i;

    i = 0;
    printf("CMD");
    while (node->argv && node->argv[i])
    {
        printf(" \"%s\"", node->argv[i]);
        i++;
    }
    
    print_list_redir(node->redir);
    
    printf("\n");
}

/**
 * print_ast_pipe
 * ----------------
 * 目的：
 *   打印管道节点 (NODE_PIPE) 的信息，并递归打印左右子节点。
 *
 * 参数：
 *   - node  : 指向 AST PIPE 节点
 *   - depth : 当前树深度，用于格式化输出（可用于缩进显示）
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 行为说明：
 *   1. 打印节点类型 "PIPE"
 *   2. 递归打印左子节点（depth + 1）
 *   3. 递归打印右子节点（depth + 1）
 */
void print_ast_pipe(ast *node, int depth)
{
    printf("PIPE\n");
    print_ast(node->left, depth + 1);
    print_ast(node->right, depth + 1);
}

/**
 * print_ast_subshell
 * ----------------
 * 目的：
 *   打印子 shell 节点 (NODE_SUBSHELL) 的信息，
 *   并递归打印其内部的 AST。
 *
 * 参数：
 *   - node  : 指向 AST SUBSHELL 节点
 *   - depth : 当前树深度，用于格式化输出（可用于缩进显示）
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 行为说明：
 *   1. 打印节点类型 "SUBSHELL"
 *   2. 递归打印子 shell 内部 AST，深度加 1
 */
void print_ast_subshell(ast *node, int depth)
{
    printf("SUBSHELL\n");
    print_ast(node->sub, depth + 1);
}
