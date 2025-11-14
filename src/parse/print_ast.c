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

/*
** print_ast
** ----------------
** 通用入口函数，用于打印任意 AST 节点。
** 根据节点类型调用不同的打印函数。
**
** 参数：
**   - node  : 当前 AST 节点
**   - depth : 树的当前深度，用于缩进显示
*/
void print_ast(ast *node, int depth)
{
    if (!node)
        return; // 空节点直接返回

    print_indent(depth);            // 打印缩进
    print_ast_by_type(node, depth); // 根据节点类型打印内容
}

/*
** print_indent
** ----------------
** 根据深度打印缩进，用于显示树的层级结构。
**
** 参数：
**   - depth : 当前 AST 树深度
**
** 每层使用两个空格进行缩进。
*/
void print_indent(int depth)
{
    int i = 0;
    while (i < depth)
    {
        printf("  "); // 每层两个空格
        i++;
    }
}

/*
** print_ast_by_type
** ----------------
** 根据 AST 节点类型选择对应的打印函数。
**
** 参数：
**   - node  : 当前 AST 节点
**   - depth : 当前树深度，用于缩进显示
**
** 支持的节点类型：
**   - NODE_CMD      : 普通命令
**   - NODE_PIPE     : 管道
**   - NODE_AND      : 逻辑与
**   - NODE_OR       : 逻辑或
**   - NODE_SUBSHELL : 子 shell
**
**
** 对于未知类型，会打印 UNKNOWN NODE TYPE。
*/
void print_ast_by_type(ast *node, int depth)
{
    /*===== printing: safe dispatcher =====*/
   
    
        if (!node)
        {
            printf("%*s(EMPTY)\n", depth * 2, "");
            return;
        }

        switch (node->type)
        {
        case NODE_CMD:
            print_ast_cmd(node);
            break;
        case NODE_PIPE:
            print_ast_pipe(node, depth);
            break;
        case NODE_AND:
            print_ast_and(node, depth);
            break;
        case NODE_OR:
            print_ast_or(node, depth);
            break;
        case NODE_SUBSHELL:
            print_ast_subshell(node, depth);
            break;
        default:
            printf("%*sUnknown AST node type %d\n", depth * 2, "", node->type);
            break;
        }
    
}
