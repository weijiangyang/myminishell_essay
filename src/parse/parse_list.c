/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 14:33:01 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/13 14:33:04 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

ast *parse_list(t_lexer **cur)
{
    // 解析左侧的“and/or”表达式
    // parse_and_or 会返回一个 AST 节点，表示一个简单命令或逻辑组合
    ast *left = parse_and_or(cur);
    if (!left) 
        return NULL; // 如果左侧解析失败，返回 NULL，表示语法错误或没有命令

    // 循环处理分号 (;) 或后台 (&) 操作符形成的命令序列
    while (peek_token(cur) && // 确保当前有下一个 token
           (peek_token(cur)->tokentype == TOK_SEMI || // 分号序列
            peek_token(cur)->tokentype == TOK_AMP))  // 后台执行
    {
        // 消耗当前 token，取出操作符节点
        t_lexer *op = consume_token(cur);

        // 为新的 AST 节点分配内存，用于表示序列或后台执行
        ast *node = ft_calloc(1, sizeof(ast));

        // 根据操作符类型设置节点类型
        node->type = (op->tokentype == TOK_AMP) ? NODE_BACKGROUND : NODE_SEQUENCE;

        // 左子树指向已经解析好的左侧命令
        node->left = left;

        // 右子树递归解析剩余的命令序列
        node->right = parse_list(cur);

        // 更新 left 指针，将新节点作为当前左侧 AST，继续可能的循环
        left = node;
    }

    // 返回解析好的 AST 节点，可能是一个序列、后台执行，或者单个命令
    return left;
}
