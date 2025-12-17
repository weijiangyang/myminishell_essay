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

/**
 * parse_cmdline
 * ----------------
 * 目的：
 *   解析完整的命令行输入，构建对应的 AST（抽象语法树）。
 *   主要处理管道、逻辑操作和简单命令的组合。
 *
 * 参数：
 *   - cur : 指向当前 token 游标的指针，用于遍历 token 链表
 *
 * 返回值：
 *   - 成功：返回解析好的 AST 根节点指针
 *   - 失败：语法错误或解析失败时返回 NULL，并释放已分配的 AST
 *
 * 行为说明：
 *   1. 调用 parse_pipeline() 解析整个命令行的管道结构
 *   2. 检查解析完成后是否还有剩余 token
 *      - 如果存在且不是 TOK_END，打印语法错误并释放 AST
 *   3. 返回 AST 根节点
 */
ast *parse_cmdline(t_lexer **cur, t_minishell *minishell)
{
    ast *root;
    t_lexer *pt;

      
    root = parse_pipeline(cur, minishell);
    if (!root)
        return NULL;
    pt = peek_token(cur);
    if (pt && pt->tokentype != TOK_END)
    {
        fprintf(stderr, "Syntax error: unexpected token at end (type %d)\n", pt->tokentype);
        free_ast(root);
        return NULL;
    }
    return root;
}
