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
#include "../../libft/libft.h"

/**
 * peek_token
 * ----------------
 * 目的：
 *   查看当前 token 游标指向的 token，但不移动游标。
 *
 * 参数：
 *   - cur : 指向当前 token 游标的指针
 *
 * 返回值：
 *   - 成功：返回当前 token 指针 (*cur)
 *   - 失败：如果 cur 为 NULL，返回 NULL
 *
 * 行为说明：
 *   - 不修改 cur 指向的 token，便于在解析时预览下一个 token
 */
t_lexer *peek_token(t_lexer **cur)
{
    if (!cur)
        return NULL;
    return *cur;
}

/**
 * consume_token
 * ----------------
 * 目的：
 *   消耗当前 token，并将游标移动到下一个 token。
 *
 * 参数：
 *   - cur : 指向当前 token 游标的指针
 *
 * 返回值：
 *   - 成功：返回被消耗的 token 指针
 *   - 失败：如果 cur 或 *cur 为 NULL，返回 NULL
 *
 * 行为说明：
 *   1. 保存当前 token 指针到临时变量 old
 *   2. 将游标 *cur 移动到下一个 token
 *   3. 返回原 token 指针 old
 */
t_lexer *consume_token(t_lexer **cur)
{
    if (!cur || !*cur)
        return NULL;

    t_lexer *old = *cur;      
    *cur = (*cur)->next;     
    return old;               
}

/**
 * expect_token
 * ----------------
 * 目的：
 *   检查当前 token 是否符合预期类型，如果符合则消耗它，
 *   否则打印语法错误并返回 NULL。
 *
 * 参数：
 *   - type : 期望的 token 类型
 *   - cur  : 指向当前 token 游标的指针
 *
 * 返回值：
 *   - 成功：返回当前 token 指针（已消耗）
 *   - 失败：当前 token 不存在或类型不匹配时返回 NULL
 *
 * 行为说明：
 *   1. 检查 cur 或 *cur 是否为 NULL，或 token 类型是否不匹配
 *   2. 如果不匹配，打印语法错误信息
 *   3. 如果匹配，调用 consume_token 消耗当前 token 并返回
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

/**
 * is_redir_token
 * ----------------
 * 目的：
 *   判断给定 token 是否为重定向类型（<, >, >>, <<）。
 *
 * 参数：
 *   - pt : 指向当前 token
 *
 * 返回值：
 *   - 1 : token 是重定向类型
 *   - 0 : token 不是重定向类型
 *
 * 行为说明：
 *   - 检查 token 的 tokentype 是否属于以下类型：
 *       TOK_REDIR_IN, TOK_REDIR_OUT, TOK_APPEND, TOK_HEREDOC
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

/**
 * safe_strdup
 * ----------------
 * 目的：
 *   安全地复制字符串，封装 strdup 并处理内存分配失败。
 *
 * 参数：
 *   - s : 需要复制的源字符串
 *
 * 返回值：
 *   - 成功：返回新分配的字符串副本
 *   - 失败或 s 为 NULL：返回 NULL
 *
 * 行为说明：
 *   1. 如果输入字符串 s 为 NULL，直接返回 NULL
 *   2. 调用 ft_strdup 复制字符串
 *   3. 如果分配失败，打印错误信息
 *   4. 返回复制后的字符串指针
 */
char *safe_strdup(const char *s)
{
    if (!s) return NULL;
    char *p = ft_strdup(s);
    if (!p)
        fprintf(stderr, "memory error: strdup failed\n");
    return p;
}
