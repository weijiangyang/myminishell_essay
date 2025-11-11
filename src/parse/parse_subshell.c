/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_subshell.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:30:13 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:30:14 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
** parse_subshell
** ----------------
** 解析子 shell 命令，即被圆括号包围的命令：
**   ( command )
**
** 子 shell 会在 AST 中生成一个 NODE_SUBSHELL 节点，
** 它的子节点 (sub) 指向括号内解析出的命令 AST。
**
** 参数：
**   - cur  : 当前 token 游标指针
**   - node : 已分配好的 AST 节点，用于存放子 shell 信息
**
** 返回：
**   - 成功：返回包含子 shell 的 AST 节点
**   - 失败：语法错误或内存释放后返回 NULL
*/
ast *parse_subshell(t_lexer **cur, ast *node)
{
    // 消耗左括号 '(' token
    consume_token(cur);

    // 设置节点类型为子 shell
    node->type = NODE_SUBSHELL;

    // 解析括号内的命令（支持 AND/OR 和管道等复杂结构）
    node->sub = parse_and_or(cur);

    // 期望右括号 ')'，否则语法错误
    if (!expect_token(TOK_RPAREN, cur))
    {
        fprintf(stderr, "Syntax error: expected ')'\n");

        // 释放子 shell 节点及其子树，防止内存泄漏
        free_ast(node);
        return NULL;
    }

    // 返回完整的子 shell AST 节点
    return node;
}
