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
    ast *right;
    t_lexer *pt;

    while (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
    {
        // 检查是否是连续的管道符号
        pt = peek_token(cur);
        if (pt && pt->next && pt->next->tokentype == TOK_PIPE) // 连续的管道符号
        {
            ft_putstr_fd("bash: syntax error near unexpected token `|'\n", STDERR_FILENO);
            return (free_ast(*left), NULL);
        }

        consume_token(cur);  // 消耗管道符号

        // 解析管道右侧的命令
        right = parse_simple_cmd_redir_list(cur, minishell);

        // 如果右侧命令为空，提示用户继续输入
        while (!right) // 如果没有右侧命令，继续等待输入
        {
            //ft_putstr_fd("Error: expected command after pipe. Waiting for input...\n", STDERR_FILENO);
            
            // 提示用户输入右侧命令
            char *buf = readline("> ");
            if (!buf)  // 如果用户按下 Ctrl+D 退出
            {
                printf("exit\n");
                break;
            }

            // 创建新的 t_minishell 结构体并解析输入
            t_minishell *test = calloc(1, sizeof(t_minishell));
            if (!test) {
                free(buf);
                return (free_ast(*left), NULL);  // 内存分配失败，释放内存并返回
            }
            test->raw_line = buf;
            handle_lexer(test);  // 处理 lexer

            // 继续解析右侧命令
            right = parse_simple_cmd_redir_list(&(test->lexer), minishell);
            if (!right) {
                free(buf);
                free(test);
                continue;  // 如果右侧命令还是为空，继续提示用户输入
            }

            free(test);  // 使用完后释放
        }

        // 创建管道节点并连接左/右命令
        ast *node = ft_calloc(1, sizeof(ast));
        if (!node) {
            free_ast(*left);
            free_ast(right);
            return NULL;
        }
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
    {
        printf("bash: syntax error near unexpected token `|'\n");
        return NULL;
    }

    n_pipes = 0;
    ast *result = parse_pipeline_1(cur, &left, &n_pipes, minishell);
    left->n_pipes = n_pipes;
    return result;
}
