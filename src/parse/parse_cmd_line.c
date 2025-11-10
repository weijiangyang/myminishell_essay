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
**   cmdline : and_or TOK_END
**
** 它会调用 parse_and_or() 来构建完整的语法树（AST），
** 然后检查是否正确到达输入结尾（防止多余 token）。
**
** 参数：
**   - cur : 指向当前 token 指针的指针（即词法分析器的游标）
**
** 返回值：
**   - 成功：返回完整语法树 (AST*)
**   - 失败：返回 NULL
*/
ast *parse_cmdline(t_lexer **cur)
{
    ast *root;     // 整个命令行的 AST 根节点
    t_lexer *pt;   // 用于检查当前 token

    // 调用下一级解析函数，解析由 AND/OR/Pipeline/Command 组成的语句结构
    root = parse_and_or(cur);
    if (!root)
        // 若解析失败（例如语法错误或内存分配失败），直接返回 NULL
        return NULL;

    // 查看当前游标指向的下一个 token（但不移动指针）
    pt = peek_token(cur);

    // 如果还有 token 且不是表示行结束的 TOK_END（即多余内容），则报语法错误
    if (pt && pt->tokentype != TOK_END)
    {
        fprintf(stderr, "Syntax error: unexpected token at end (type %d)\n", pt->tokentype);
        // 清理 AST，防止内存泄漏
        free_ast(root);
        return NULL;
    }

    // 若成功解析完整命令且无多余 token，则返回 AST 根节点
    return root;
}









