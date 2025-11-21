/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_simple_cmd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:29:58 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:30:01 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

/**
 * parse_cmd_argv
 * ----------------
 * 目的：
 *   将单个命令参数 token 添加到 AST 命令节点的 argv 数组中，
 *   并在必要时动态扩容 argv。
 *
 * 参数：
 *   - cur      : 指向当前 token 游标的指针
 *   - node     : 指向 AST 命令节点
 *   - argv_cap : 指向当前 argv 数组容量的指针（可能扩容）
 *   - argc     : 指向当前参数数量的指针（会递增）
 *
 * 返回值：
 *   - 成功：返回更新后的 AST 节点
 *   - 失败：返回 NULL（token 缺失或内存分配失败）
 *
 * 行为说明：
 *   1. 从 cur 获取当前 token（命令参数）
 *   2. 如果 argv 数组容量不足，使用 realloc 扩容为原来的两倍
 *   3. 使用 safe_strdup 复制 token 字符串，添加到 argv[*argc]
 *   4. 更新 argc
 */
/*static ast *parse_cmd_argv(t_lexer **cur, ast *node, size_t *argv_cap, size_t *argc)
{
    t_lexer *t;
    char **tmp;
    char *s;

    if (!cur || !*cur || !node || !argv_cap || !argc)
        return NULL;
    t = consume_token(cur);
    if (!t)
        return NULL;
    if (*argc + 1 >= *argv_cap)
    {
        *argv_cap *= 2;
        tmp = realloc(node->argv, (*argv_cap) * sizeof(char *));
        if (!tmp)
            return (fprintf(stderr, "realloc failed\n"), NULL);
        node->argv = tmp;
    }
    s = safe_strdup(t->str);
    if (!s)
        return (NULL);
    node->argv[*argc] = s;
    (*argc)++;
    return node;
}*/

/**
 * parse_normal_cmd_redir_1
 * ----------------
 * 目的：
 *   解析普通命令中紧随命令名后的参数和重定向，
 *   将参数添加到 argv，重定向信息填入 AST 节点。
 *
 * 参数：
 *   - cur      : 指向当前 token 游标的指针
 *   - node     : 指向 AST 命令节点
 *   - argv_cap : 指向 argv 数组容量的指针（可能扩容）
 *   - argc     : 指向当前参数数量的指针（递增）
 *
 * 返回值：
 *   - 成功：返回更新后的 AST 节点
 *   - 失败：返回 NULL（解析失败或内存分配失败）
 *
 * 行为说明：
 *   1. 循环检查下一个 token：
 *        - 如果是普通单词 (TOK_WORD)，调用 parse_cmd_argv 添加到 argv
 *        - 如果是重定向 token，调用 parse_pre_redir 处理重定向
 *        - 否则退出循环
 *   2. 循环结束后，argv[*argc] 设置为 NULL，方便 execvp 调用
 */
/*static ast *parse_normal_cmd_redir_1(t_lexer **cur, ast *node, size_t *argv_cap, size_t *argc)
{
    t_lexer *pt;

    while ((pt = peek_token(cur)) != NULL)
    {
        if (pt->tokentype == TOK_WORD)
        {
            if (!parse_cmd_argv(cur, node, argv_cap, argc))
                return (NULL);
        }
        else if (is_redir_token(pt))
        {
            if (!parse_pre_redir(cur, node))
                return NULL;
        }
        else
            break;
    }
    node->argv[*argc] = NULL;
    return (node);
}*/

/**
 * parse_normal_cmd_redir
 * ----------------
 * 目的：
 *   解析普通命令的命令名及其参数，同时处理可能存在的重定向，
 *   并初始化 AST 节点的 argv 数组。
 *
 * 参数：
 *   - cur  : 指向当前 token 游标的指针
 *   - node : 指向 AST 命令节点
 *
 * 返回值：
 *   - 成功：返回填充好 argv 和重定向信息的 AST 节点
 *   - 失败：返回 NULL（语法错误或内存分配失败）
 *
 * 行为说明：
 *   1. 初始化 argv 数组初始容量为 8
 *   2. 检查 token 游标和节点是否有效
 *   3. 获取当前 token 并分配 argv 数组
 *   4. 确认第一个 token 是命令名 (TOK_WORD)，否则报语法错误
 *   5. 调用 parse_normal_cmd_redir_1 解析后续参数和重定向
 */
/*static ast *parse_normal_cmd_redir(t_lexer **cur, ast *node)
{
    size_t argv_cap;
    size_t argc;
    t_lexer *pt;

    argv_cap = 8;
    argc = 0;
    if (!cur || !node)
        return NULL;
    pt = peek_token(cur);
    node->argv = ft_calloc(argv_cap, sizeof(char *));
    if (!node->argv)
        return (fprintf(stderr, "calloc failed\n"), NULL);
    if (!pt || (pt->tokentype != TOK_WORD))
        return (fprintf(stderr, "Syntax error: expected command name\n"), NULL);
    return (parse_normal_cmd_redir_1(cur, node, &argv_cap, &argc));
}*/

/**
 * parse_normal_cmd
 * ----------------
 * 目的：
 *   解析普通命令，包括前置重定向、命令名、参数以及中间或后续的重定向，
 *   并填充 AST 节点。
 *
 * 参数：
 *   - cur  : 指向当前 token 游标的指针
 *   - node : 已分配的 AST 命令节点
 *
 * 返回值：
 *   - 成功：返回完整填充的 AST 命令节点
 *   - 失败：返回 NULL（解析失败或语法错误）
 *
 * 行为说明：
 *   1. 检查参数有效性
 *   2. 将节点类型设置为 NODE_CMD
 *   3. 循环处理命令前的重定向 token
 *   4. 检查是否到达输入结束 (TOK_END)
 *        - 如果是，则返回仅包含重定向的节点
 *   5. 调用 parse_normal_cmd_redir 处理命令名、参数及可能的后续重定向
 */
/*static ast *parse_normal_cmd(t_lexer **cur, ast *node)
{
    t_lexer *pt;

    if (!cur || !node)
        return NULL;
    node->type = NODE_CMD;
    while ((pt = peek_token(cur)) != NULL && is_redir_token(pt))
    {
        if (!parse_pre_redir(cur, node))
            return (NULL);
    }
    pt = peek_token(cur);
    if (!pt || pt->tokentype == TOK_END)
        return (node);
    node = parse_normal_cmd_redir(cur, node);
    return node;
}
*/
/**
 * parse_simple_cmd
 * ----------------
 * 目的：
 *   解析一个简单命令或子 shell，并返回对应的 AST 节点。
 *
 * 参数：
 *   - cur : 指向当前 token 游标的指针
 *
 * 返回值：
 *   - 成功：返回解析好的 AST 节点（普通命令或子 shell）
 *   - 失败：返回 NULL（内存分配失败或解析错误）
 *
 * 行为说明：
 *   1. 获取当前 token
 *   2. 分配新的 AST 节点
 *   3. 如果当前 token 是左括号 '('，调用 parse_subshell 解析子 shell
 *   4. 否则调用 parse_normal_cmd 解析普通命令
 */
/*ast *parse_simple_cmd(t_lexer **cur)
{
    ast *node;
    t_lexer *pt;

    pt = peek_token(cur);
    node = ft_calloc(1, sizeof(ast));
    if (!node)
        return (NULL);
    if (pt && pt->tokentype == TOK_LPAREN)
        return (parse_subshell(cur, node));
    return (parse_normal_cmd(cur, node));
}
*/