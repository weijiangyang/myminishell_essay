/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:31:17 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:31:19 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/*
** peek_token
** ----------------
** 查看当前 token，但不移动游标。
**
** 参数：
**   - cur : 当前 token 的指针（token 游标）
**
** 返回：
**   - 当前 token 指针，如果 cur 为 NULL 或指向 NULL，返回 NULL
*/
t_lexer *peek_token(t_lexer **cur)
{
    if (!cur)
        return NULL;
    return *cur;
}

/*
** consume_token
** ----------------
** 消耗当前 token，并将游标前进到下一个 token。
**
** 参数：
**   - cur : 当前 token 的指针（token 游标）
**
** 返回：
**   - 被消耗的 token 指针
**   - 如果 cur 或 *cur 为 NULL，返回 NULL
*/
t_lexer *consume_token(t_lexer **cur)
{
    if (!cur || !*cur)
        return NULL;

    t_lexer *old = *cur;      // 保存当前 token
    *cur = (*cur)->next;      // 游标前进
    return old;               // 返回消耗的 token
}

/*
** expect_token
** ----------------
** 确认当前 token 类型与期望类型一致，如果匹配则消耗并返回该 token。
** 否则打印语法错误并返回 NULL。
**
** 参数：
**   - type : 期望的 token 类型
**   - cur  : 当前 token 的指针（token 游标）
**
** 返回：
**   - 成功：返回消耗的 token
**   - 失败：打印错误并返回 NULL
*/
t_lexer *expect_token(tok_type type, t_lexer **cur)
{
    if (!cur || !*cur || (*cur)->tokentype != type)
    {
        fprintf(stderr, "Syntax error : expected token type %d\n", type);
        return NULL;
    }
    return consume_token(cur);
}

/*
** is_redir_token
** ----------------
** 判断当前 token 是否是重定向类型：
**   - 输入重定向 (<)
**   - 输出重定向 (>)
**   - 输出追加 (>>)
**   - heredoc (<<)
**
** 参数：
**   - pt : 当前 token 指针
**
** 返回：
**   - 1 : 是重定向 token
**   - 0 : 不是重定向 token
*/
int is_redir_token(t_lexer *pt)
{
    if (pt->tokentype == TOK_REDIR_IN || 
        pt->tokentype == TOK_REDIR_OUT || 
        pt->tokentype == TOK_APPEND || 
        pt->tokentype == TOK_HEREDOC)
        return 1;
    else
        return 0;
}
