/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:29:46 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:29:47 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * process_redir_1
 * ----------------
 * 目的：
 *   根据重定向类型，将临时字符串 tmp 分配给 AST 命令节点
 *   的对应重定向字段，并释放旧的值。
 *
 * 参数：
 *   - type : 重定向类型（TOK_REDIR_IN, TOK_REDIR_OUT, TOK_APPEND, TOK_HEREDOC）
 *   - node : 指向 AST 命令节点
 *   - tmp  : 已分配的字符串，表示重定向目标（由调用者提供）
 *
 * 返回值：
 *   - 0  : 成功设置重定向
 *   - -1 : 未知或非法重定向类型，tmp 被释放
 *
 * 行为说明：
 *   1. 根据 type 判断是输入、输出、追加还是 heredoc 重定向
 *   2. 释放节点中已有的同类型字符串（避免内存泄漏）
 *   3. 将 tmp 赋值给节点对应的重定向字段
 *   4. 若 type 不合法，释放 tmp 并返回 -1
 */
int process_redir_1(tok_type type, ast *node, char *tmp)
{
    if (type == TOK_REDIR_IN)
    {
        free(node->redir_in);
        node->redir_in = tmp;
    }
    else if (type == TOK_REDIR_OUT)
    {
        free(node->redir_out);
        node->redir_out = tmp;
    }
    else if (type == TOK_APPEND)
    {
        free(node->redir_append);
        node->redir_append = tmp;
    }
    else if (type == TOK_HEREDOC)
    {
        free(node->heredoc_delim);
        node->heredoc_delim = tmp;
    }
    else
        return (free(tmp), -1);
    return (0);
}

/**
 * process_redir
 * ----------------
 * 目的：
 *   处理单个重定向操作，将 token 指向的文件名复制到 AST 命令节点
 *   的对应重定向字段。
 *
 * 参数：
 *   - redir : 指向重定向操作符 token（如 '<', '>', '>>', '<<'）
 *   - file  : 指向重定向目标文件名 token
 *   - node  : 指向 AST 命令节点
 *
 * 返回值：
 *   - 0  : 成功处理重定向
 *   - -1 : 参数非法或内存分配失败
 *
 * 行为说明：
 *   1. 检查 redir、file 和 node 是否为 NULL
 *   2. 使用 safe_strdup 复制文件名字符串，确保分配安全
 *   3. 调用 process_redir_1 将 tmp 设置到 AST 节点对应字段
 *      并释放旧值（若存在）
 *   4. 返回 process_redir_1 的执行结果
 */
int process_redir(t_lexer *redir, t_lexer *file, ast *node)
{
    char *tmp;

    if (!redir || !file || !node)
        return -1;
    tmp = safe_strdup(file->str);
    if (!tmp)
        return -1;
    return (process_redir_1(redir->tokentype, node, tmp));
}

/**
 * parse_pre_redir
 * ----------------
 * 目的：
 *   解析单个前置重定向（命令前或命令中间出现的 '<', '>', '>>', '<<'）
 *   并将目标文件或 heredoc 保存到 AST 命令节点中。
 *
 * 参数：
 *   - cur  : 指向当前 token 游标的指针
 *   - node : 指向 AST 命令节点
 *
 * 返回值：
 *   - 成功：返回更新后的 node
 *   - 失败：返回 NULL（语法错误或内部错误），由调用者负责释放 node
 *
 * 行为说明：
 *   1. 从 cur 获取重定向操作符 token
 *      - 若缺失 token，打印内部错误并返回 NULL
 *   2. 获取重定向目标 token（必须为 TOK_WORD）
 *      - 若缺失，打印语法错误并返回 NULL
 *   3. 调用 process_redir 将目标字符串设置到 AST 节点对应字段
 *      - 若失败，打印内部错误并返回 NULL
 *   4. 返回更新后的 node
 */
ast *parse_pre_redir(t_lexer **cur, ast *node)
{
    t_lexer *redir;
    t_lexer *file;

    if (!cur || !*cur || !node)
        return NULL;
    redir = consume_token(cur);
    if (!redir)
    {
        fprintf(stderr, "internal parser error: missing redir token\n");
        return NULL;
    }
    file = expect_token(TOK_WORD, cur);
    if (!file)
    {
        fprintf(stderr, "Syntax error: expected filename after redirection\n");
        return NULL;
    }
    if (process_redir(redir, file, node) < 0)
    {
        fprintf(stderr, "internal error: failed to process redirection\n");
        return NULL;
    }
    return node;
}
