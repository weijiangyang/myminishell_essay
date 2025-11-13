/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yang <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 22:19:34 by yang              #+#    #+#             */
/*   Updated: 2025/10/31 22:19:38 by yang             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*  
** parse_cmdline
** ----------------
** 这是整个命令解析过程的“最高层入口函数”，负责解析完整的一行命令。
**
** 语法规则（简化版）：
**   cmdline : list TOK_END
**   list    : and_or { ( ; | & ) list }
**
** 它会调用 parse_list() 来构建完整的语法树（AST），
** 然后检查是否正确到达输入结尾（防止多余 token）。
**
** 参数：
**   - cur : 指向当前 token 指针的指针（即 lexer 的游标）
**
** 返回值：
**   - 成功：返回完整语法树 (AST*)
**   - 失败：返回 NULL
*/
ast *parse_cmdline(t_lexer **cur)
{
    ast *root;     // 整个命令行的 AST 根节点
    t_lexer *pt;   // 用于查看当前 token，但不移动游标

    // 调用下一级解析函数 parse_list() 来解析由 AND/OR/Pipeline/Command 组成的命令序列
    root = parse_list(cur);
    if (!root)
        // 若解析失败（例如语法错误、内存分配失败或空输入），直接返回 NULL
        return NULL;

    // peek 当前 token，看是否有多余的 token
    pt = peek_token(cur);

    // 如果还有 token 且不是行结束标志 TOK_END，则说明命令行有语法错误
    if (pt && pt->tokentype != TOK_END)
    {
        fprintf(stderr, "Syntax error: unexpected token at end (type %d)\n", pt->tokentype);
        // 释放已经解析的 AST，防止内存泄漏
        free_ast(root);
        return NULL;
    }

    // 成功解析完整命令且没有多余 token，返回 AST 根节点
    return root;
}










