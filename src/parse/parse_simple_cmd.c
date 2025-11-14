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
/*
 * parse_cmd_argv - returns node on success, NULL on error (do not use node after NULL)
 */
static ast *parse_cmd_argv(t_lexer **cur, ast *node, size_t *argv_cap, size_t *argc)
{
    t_lexer *t;
    char **tmp;
    char *s;

    if (!cur || !*cur || !node || !argv_cap || !argc)
        return NULL;

    t = consume_token(cur);
    if (!t)
        return NULL;

    /* expand argv if needed */
    if (*argc + 1 >= *argv_cap)
    {
        size_t newcap = (*argv_cap) * 2;
        tmp = realloc(node->argv, newcap * sizeof(char *));
        if (!tmp)
        {
            fprintf(stderr, "realloc failed\n");
            return NULL;
        }
        node->argv = tmp;
        *argv_cap = newcap;
    }

    s = safe_strdup(t->str);
    if (!s)
        return (NULL);
    node->argv[*argc] = s;
    (*argc)++;
    return node;
}
/*
 * parse_normal_cmd_redir - now returns ast* on success or NULL on error.
 * Caller must free node on error.
 */
static ast *parse_normal_cmd_redir(t_lexer **cur, ast *node, t_lexer *pt)
{
    size_t argv_cap = 8;
    size_t argc = 0;
    t_lexer *t;

    if (!cur || !node)
        return NULL;

    node->argv = ft_calloc(argv_cap, sizeof(char *));
    if (!node->argv)
    {
        fprintf(stderr, "calloc failed\n");
        return NULL;
    }

    /* pt should be a WORD (command name) */
    if (!pt || (pt->tokentype != TOK_WORD))
    {
        fprintf(stderr, "Syntax error: expected command name\n");
        return NULL;
    }

    /* consume command name */
    t = consume_token(cur);
    if (!t)
        return NULL;
    node->argv[argc] = safe_strdup(t->str);
    argc++;
    if (!node->argv[argc - 1])
        return NULL;
    /* parse subsequent tokens */
    while ((pt = peek_token(cur)) != NULL)
    {
        if (pt->tokentype == TOK_WORD)
        {
            if (!parse_cmd_argv(cur, node, &argv_cap, &argc))
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
    /* NULL-terminate argv */
    if (argc >= argv_cap)
    {
        /* ensure space for NULL */
        char **tmp = realloc(node->argv, (argv_cap + 1) * sizeof(char *));
        if (!tmp)
            return NULL;
        node->argv = tmp;
        argv_cap++;
    }
    node->argv[argc] = NULL;
    return node;
}

/*
 * parse_normal_cmd - protect against NULL peek_token and propagate errors
 */
static ast *parse_normal_cmd(t_lexer **cur, ast *node)
{
    t_lexer *pt;

    if (!cur || !node)
        return NULL;

    node->type = NODE_CMD;

    /* leading redirections */
    while ((pt = peek_token(cur)) != NULL && is_redir_token(pt))
    {
        if (!parse_pre_redir(cur, node))
        {
            /* syntax error: caller should free node */
            return NULL;
        }
    }

    pt = peek_token(cur);
    if (!pt || pt->tokentype == TOK_END)
        return (node); /* input finished. treat as empty command (allowed if redirection present) */
    /* parse command name + args + mid redirs */
    node = parse_normal_cmd_redir(cur, node, pt);
    return node; /* may be NULL on error */
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

    pt = peek_token(cur);             // 查看当前 token
    node = ft_calloc(1, sizeof(ast)); // 分配 AST 节点
    if (!node)
        return (NULL);
    // 如果是左括号，则解析为子 shell
    if (pt && pt->tokentype == TOK_LPAREN)
        return (parse_subshell(cur, node));
    // 否则解析为普通命令
    return (parse_normal_cmd(cur, node));
}
