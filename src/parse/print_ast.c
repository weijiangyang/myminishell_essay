/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ast.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:31:08 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:31:09 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * print_ast
 * ----------------
 * 目的：
 *   根据 AST 节点类型打印节点信息，并按树的深度缩进。
 *
 * 参数：
 *   - node  : 指向 AST 节点
 *   - depth : 当前树深度，用于缩进显示
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 行为说明：
 *   1. 如果节点为空，直接返回
 *   2. 调用 print_indent 根据 depth 打印缩进
 *   3. 调用 print_ast_by_type 根据节点类型打印详细信息
 */
void print_ast(ast *node, int depth)
{
    if (!node)
        return;
    print_indent(depth);
    print_ast_by_type(node, depth);
}

/**
 * print_indent
 * ----------------
 * 目的：
 *   根据树的深度打印缩进，用于格式化 AST 输出。
 *
 * 参数：
 *   - depth : 当前树深度，决定缩进的层数
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 行为说明：
 *   1. 循环 depth 次，每次打印两个空格作为缩进
 */
void print_indent(int depth)
{
    int i;

    i = 0;
    while (i < depth)
    {
        printf("  ");
        i++;
    }
}

/**
 * print_ast_by_type
 * ----------------
 * 目的：
 *   根据 AST 节点类型打印节点详细信息。
 *
 * 参数：
 *   - node  : 指向 AST 节点
 *   - depth : 当前树深度，用于格式化输出（缩进）
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 行为说明：
 *   1. 如果节点为空，打印 "(EMPTY)" 并带缩进
 *   2. 根据节点类型选择不同的打印函数：
 *        - NODE_CMD      : 调用 print_ast_cmd
 *        - NODE_PIPE     : 调用 print_ast_pipe
 *        - NODE_SUBSHELL : 调用 print_ast_subshell
 *   3. 如果节点类型未知，打印 "Unknown AST node type"
 */
void print_ast_by_type(ast *node, int depth)
{
    if (!node)
    {
        printf("%*s(EMPTY)\n", depth * 2, "");
        return;
    }
    if (node->type == NODE_CMD)
        print_ast_cmd(node);
    else if (node->type == NODE_PIPE)
        print_ast_pipe(node, depth);
    else if (node->type == NODE_SUBSHELL)
        print_ast_subshell(node, depth);
    else
        printf("%*sUnknown AST node type %d\n", depth * 2, "", node->type);
}
