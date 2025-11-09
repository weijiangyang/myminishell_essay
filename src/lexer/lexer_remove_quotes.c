/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_remove_quotes.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/28 05:16:37 by yzhang2           #+#    #+#             */
/*   Updated: 2025/10/28 07:19:36 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// 作用：从源串 `s` 的当前读指针起，拷贝**被指定引号包裹**的内容到 `res`，
// 并维护源/目标索引 `idx`。
// 参数：源串、结果缓冲、`idx`（含 `i`/`j`）、引号字符。
// 逻辑：跳过开引号→复制中间内容直到遇到同类闭引号→跳过闭引号。
static void	copy_quoted_block(const char *s, char *res, t_index *idx,
		char quote)
{
	if (s[idx->i] == quote)
	{
		(idx->i)++;
		while (s[idx->i] && s[idx->i] != quote)
			res[(idx->j)++] = s[(idx->i)++];
		if (s[idx->i] == quote)
			(idx->i)++;
	}
}

// 作用：不含引号时，直接把 `s` 内容复制到 `res`。
// 参数：源串、目标串。
// 逻辑：线性复制并返回写入长度。
static size_t	copy_content_no_quotes(const char *s, char *res)
{
	t_index	idx;

	idx.i = 0;
	idx.j = 0;
	while (s[idx.i])
	{
		if (s[idx.i] == '\'')
			copy_quoted_block(s, res, &idx, '\'');
		else if (s[idx.i] == '"')
			copy_quoted_block(s, res, &idx, '"');
		else
			res[idx.j++] = s[idx.i++];
	}
	return (idx.j);
}

// 作用：预扫描 `s`，记录单/双引号出现情况。
// 参数：源串、两个标志输出位。
// 逻辑：遍历标记，结果用于决TOK_ERROR定是否需要引号处理与返回何种标记。
static int	check_and_set_flags(const char *s, int *q_single, int *q_double)
{
	size_t	i;

	i = 0;
	*q_single = 0;
	*q_double = 0;
	while (s[i])
	{
		if (s[i] == '\'')
			*q_single = 1;
		if (s[i] == '"')
			*q_double = 1;
		i++;
	}
	if (*q_single == 0 && *q_double == 0)
		return (0);
	return (1);
}

// 作用：去掉包裹引号，返回新串，并填充 `had_q`/`q_single`/`q_double`。
// 参数：源串与三个输出标志。
// 逻辑：先预扫字符串，标记是否出现过单/双引号；若未出现，引号标志清零并返回原指针约定。
// 若出现，则分配结果缓冲并逐字符复制：遇到引号就调用拷贝块逻辑复制引号内部内容（不含引号本身），
// 普通字符直接抄写；最后写入 \0 并设置 *had_q=1、对应的 *q_single/*q_double 标志，返回去壳后的新串。
char	*remove_quotes_flag(const char *s, int *had_q, int *q_single,
		int *q_double)
{
	size_t	len;
	size_t	j;
	char	*res;

	res = NULL;
	len = 0;
	*had_q = 0;
	if (!s)
	{
		*q_single = 0;
		*q_double = 0;
		return (NULL);
	}
	if (check_and_set_flags(s, q_single, q_double) == 0)
		return (NULL);
	len = strlen(s);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	j = copy_content_no_quotes(s, res);
	res[j] = '\0';
	*had_q = 1;
	return (res);
}
