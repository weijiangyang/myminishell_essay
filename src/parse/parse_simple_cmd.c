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

/*
** parse_cmd_argv
** ----------------
** 将单个命令参数添加到 AST 节点的 argv 数组中。
**
** 参数：
**   - cur       : 当前 token 游标指针
**   - node      : 当前命令 AST 节点
**   - argv_cap  : argv 数组当前容量指针（可能动态扩容）
**   - argc      : 当前参数个数指针
**
** 返回：
**   - 成功：返回更新后的 node
**   - 失败：分配失败时释放 node 并返回 NULL
*/
static ast *parse_cmd_argv(t_lexer **cur, ast *node, size_t *argv_cap, size_t *argc)
{
    t_lexer *t; // 当前参数 token
    char **tmp; // 临时用于 realloc 的数组指针

    t = consume_token(cur); // 取出参数 token

    // 如果 argv 数组已满，则动态扩容
    if (*argc + 1 >= *argv_cap)
    {
        *argv_cap *= 2;
        tmp = realloc(node->argv, *argv_cap * sizeof(char *));
        if (!tmp)
        {
            fprintf(stderr, "realloc failed\n");
            free_ast(node); // 内存释放
            return NULL;
        }
        node->argv = tmp;
    }

    // 复制参数字符串到 argv 数组
    node->argv[*argc] = strdup(t->str);
    (*argc)++;
    return (node);
}

/*
** parse_normal_cmd_redir
** ----------------
** 解析普通命令（非子 shell）的参数和重定向。
**
** 参数：
**   - cur : token 游标
**   - node: 当前命令 AST 节点
**   - pt  : 当前 token（通常是命令名 token）
**
** 返回：
**   - 成功：返回填充好 argv 和重定向的 node
**   - 失败：打印错误信息并释放节点
*/
static void *parse_normal_cmd_redir(t_lexer **cur, ast *node, t_lexer *pt)
{
    size_t argv_cap = 8; // 初始 argv 容量
    size_t argc = 0;     // 当前参数数量
    t_lexer *t;

    // 分配 argv 数组
    node->argv = calloc(argv_cap, sizeof(char *));
    if (!node->argv)
        return (free(node), NULL);

    // 检查第一个 token 是否是命令名
    if (!pt || pt->tokentype != TOK_WORD)
        return (fprintf(stderr, "Syntax error: expected command name\n"), free_ast(node), NULL);

    // 保存命令名
    t = consume_token(cur);
    node->argv[argc++] = strdup(t->str);

    // 解析后续 token：可能是参数或重定向
    while ((pt = peek_token(cur)) && (pt->tokentype == TOK_WORD || is_redir_token(pt)))
    {
        if (pt->tokentype == TOK_WORD)
            parse_cmd_argv(cur, node, &argv_cap, &argc); // 添加参数
        else
            parse_pre_redir(cur, node); // 处理重定向
    }

    // argv 数组以 NULL 结尾，方便执行阶段使用 execvp 等
    node->argv[argc] = NULL;
    // 👇 这里添加对 & 的检测
    pt = peek_token(cur);
    if (pt && pt->tokentype == TOK_AMP) // 假设 & 的类型是 TOK_AMP
    {
        consume_token(cur);      // 消费掉 &
        node->is_background = 1; // 标记为后台命令
    }
    else
    {
        node->is_background = 0; // 默认前台
    }
    return (node);
}

/*
** parse_normal_cmd
** ----------------
** 解析普通命令（非子 shell），包括重定向和参数。
**
** 参数：
**   - cur : token 游标
**   - node: 当前命令 AST 节点（已分配内存）
**
** 返回：
**   - 完整的命令 AST 节点
*/
static ast *parse_normal_cmd(t_lexer **cur, ast *node)
{
    t_lexer *pt;

    node->type = NODE_CMD; // 设置节点类型为普通命令
    // 处理前置重定向（命令前可能有 < input 等）
    while ((pt = peek_token(cur)) && is_redir_token(pt))
        parse_pre_redir(cur, node);
    // 处理命令名和后续参数及重定向
    parse_normal_cmd_redir(cur, node, pt);
    return (node);
}
/*
** parse_simple_cmd
** ----------------
** 解析一个简单命令：
**   - 可能是子 shell（(...)）
**   - 或普通命令（带参数和重定向）
**
** 参数：
**   - cur : token 游标
**
** 返回：
**   - 成功：返回命令或子 shell AST 节点
**   - 失败：返回 NULL
*/
ast *parse_simple_cmd(t_lexer **cur)
{
    ast *node;
    t_lexer *pt;

    pt = peek_token(cur);          // 查看当前 token
    node = calloc(1, sizeof(ast)); // 分配 AST 节点
    if (!node)
        return (NULL);
    // 如果是左括号，则解析为子 shell
    if (pt && pt->tokentype == TOK_LPAREN)
        return (parse_subshell(cur, node));
    // 否则解析为普通命令
    return (parse_normal_cmd(cur, node));
}
