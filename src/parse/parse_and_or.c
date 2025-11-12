/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_and_or.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:28:46 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:28:48 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
** parse_and_or_1
** ----------------
** 递归解析包含逻辑运算符 `&&` 与 `||` 的命令结构。
**
** 示例：
**   cmd1 && cmd2 || cmd3
**
** 语法规则：
**   and_or : pipeline ( (&& | ||) pipeline )*
**
** 参数：
**   - cur  : 当前 token 的游标（指向当前词法单元的指针）
**   - left : 已经解析好的左侧 AST 节点指针（例如第一个 pipeline）
**
** 返回值：
**   - 完整的 and/or 语法树根节点（即左侧节点）
*/
static ast *parse_and_or_1(t_lexer **cur, ast **left)
{
    t_lexer *op; // 保存当前操作符 token（&& 或 ||）
    ast *right;  // 保存右侧命令的 AST
    ast *node;   // 新建一个父节点，连接左右子树

    // 当下一个 token 是 && 或 || 时，继续构建复合语句
    while (peek_token(cur) &&
           (peek_token(cur)->tokentype == TOK_AND || peek_token(cur)->tokentype == TOK_OR))
    {
        // 取出操作符 token 并前进
        op = consume_token(cur);

        // 解析操作符右侧的 pipeline 命令
        right = parse_pipeline(cur);
        if (!right)
            // 如果右侧解析失败，释放左侧并返回 NULL，防止内存泄漏
            return (free_ast(*left), NULL);

        // 分配一个新的 AST 节点，用于表示该 && / || 结构
        node = calloc(1, sizeof(ast));
        if (!node)
            // 分配失败则释放左右子树，避免内存泄漏
            return (free_ast(*left), free_ast(right), NULL);

        // 根据操作符类型设置节点类型
        if (op->tokentype == TOK_AND)
            node->type = NODE_AND;
        else
            node->type = NODE_OR;

        // 连接左右子节点
        node->left = *left;
        node->right = right;

        // 更新 left 指针，让新节点成为当前树的根
        *left = node;
    }

    // 返回构建完成的 AST 树根节点
    return (*left);
}

/*
** parse_and_or
** ----------------
** 解析以 `&&` 或 `||` 连接的命令序列。
**
** 它首先解析一个 pipeline（命令或管道链），
** 然后调用 parse_and_or_1() 解析后续的逻辑操作。
**
** 示例：
**   cmd1 && cmd2 || cmd3
**
** 参数：
**   - cur : 当前 token 的游标指针（传入后会被移动）
**
** 返回值：
**   - 构建完成的 AST 树根节点（可能是 NODE_BACKGROUND / NODE_SEMI /NODE_AND / NODE_OR / NODE_PIPE / NODE_CMD）
*/
ast *parse_and_or(t_lexer **cur)
{
    ast *left;

    // 首先解析左侧的 pipeline（可能是一个简单命令或管道链）
    left = parse_pipeline(cur);
    if (!left)
        return NULL;

    // 继续解析后续可能存在的 && 或 || 连接部分
    parse_and_or_1(cur, &left);

    // 返回整个 and/or 语法树的根节点
    return left;
}

ast *parse_list(t_lexer **cur)
{
    ast *left = parse_and_or(cur);
    if (!left) return NULL;

    while (peek_token(cur) &&
           (peek_token(cur)->tokentype == TOK_SEMI ||
            peek_token(cur)->tokentype == TOK_AMP))
    {
        t_lexer *op = consume_token(cur);
        ast *node = calloc(1, sizeof(ast));
        node->type = (op->tokentype == TOK_AMP) ? NODE_BACKGROUND : NODE_SEQUENCE;
        node->left = left;
        node->right = parse_list(cur);
        left = node;
    }

    return left;
}
