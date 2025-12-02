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

/**
 * parse_subshell
 * ----------------
 * 目的：
 *   解析子 shell 表达式，即括号内的命令，并返回对应的 AST 节点。
 *
 * 参数：
 *   - cur  : 指向当前 token 游标的指针
 *   - node : 已分配的 AST 节点，用于存储子 shell 信息
 *
 * 返回值：
 *   - 成功：返回填充好的 NODE_SUBSHELL AST 节点
 *   - 失败：返回 NULL（语法错误或缺少右括号），并释放节点
 *
 * 行为说明：
 *   1. 消耗左括号 '(' token
 *   2. 将节点类型设置为 NODE_SUBSHELL
 *   3. 调用 parse_pipeline 解析括号内的命令序列
 *   4. 检查右括号 ')' 是否存在，若缺失打印语法错误并释放节点
 *   5. 返回子 shell AST 节点
 */
ast *parse_subshell(t_lexer **cur, ast *node, t_minishell *minishell)
{
    consume_token(cur);
    node->type = NODE_SUBSHELL;
    node->sub = parse_pipeline(cur, minishell);
    if (!expect_token(TOK_RPAREN, cur))
    {
        fprintf(stderr, "Syntax error: expected ')'\n");
        free_ast(node);
        return NULL;
    }
    return node;
}
