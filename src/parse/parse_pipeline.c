/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_pipeline.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:29:31 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:29:33 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * parse_pipeline_1
 * ----------------
 * 目的：
 *   解析管道操作符 '|'，构建对应的 PIPE 类型 AST 节点。
 *   将左侧命令与右侧命令组合成一棵二叉树表示管道链。
 *
 * 参数：
 *   - cur     : 指向当前 token 游标的指针
 *   - left    : 指向已解析的左侧 AST 节点指针（输入/输出参数）
 *   - n_pipes : 指向管道数量计数器，每遇到一个 '|' 就递增
 *
 * 返回值：
 *   - 成功：返回更新后的 AST 节点（PIPE 树的根）
 *   - 失败：语法错误或内存分配失败时返回 NULL，并释放相关 AST
 *
 * 行为说明：
 *   1. 循环检查当前 token 是否为 TOK_PIPE
 *   2. 消耗管道符 token
 *   3. 调用 parse_simple_cmd() 解析管道右侧命令
 *   4. 为管道创建一个新的 NODE_PIPE AST 节点，将左/右子树连接
 *   5. 更新 left 指针为新创建的 PIPE 节点，继续处理后续管道
 */
static ast *parse_pipeline_1(t_lexer **cur, ast **left, int *n_pipes, t_minishell *minishell)
{
    while (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
    {
        ast *right;
        ast *node;

        consume_token(cur);
        right = parse_simple_cmd_redir_list(cur, minishell);
        if (!right)
            return (free_ast(*left), NULL);
        node = ft_calloc(1, sizeof(ast));
        if (!node)
            return (free_ast(*left), free_ast(right), NULL);
        node->type = NODE_PIPE;
        node->left = *left;
        node->right = right;
        (*n_pipes)++;
        *left = node;
    }
    return (*left);
}

/**
 * parse_pipeline
 * ----------------
 * 目的：
 *   解析一条完整的管道命令，将多个通过 '|' 连接的简单命令
 *   构建成 PIPE 类型的 AST 树。
 *
 * 参数：
 *   - cur : 指向当前 token 游标的指针
 *
 * 返回值：
 *   - 成功：返回包含整个管道结构的 AST 根节点
 *   - 失败：解析失败时返回 NULL
 *
 * 行为说明：
 *   1. 首先调用 parse_simple_cmd() 解析管道最左侧的命令
 *   2. 调用 parse_pipeline_1() 解析右侧可能存在的管道，更新 AST
 *   3. 将管道数量 n_pipes 保存到 AST 根节点的 n_pipes 字段
 *   4. 返回 AST 根节点
 */
ast *parse_pipeline(t_lexer **cur, t_minishell *minishell)
{
    ast *left;
    int n_pipes;

    left = parse_simple_cmd_redir_list(cur, minishell);
    if (!left)
        return NULL;
    n_pipes = 0;
    parse_pipeline_1(cur, &left, &n_pipes, minishell);
    left->n_pipes = n_pipes;
    return left;
}
