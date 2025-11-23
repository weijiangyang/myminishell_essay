/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 07:53:56 by yzhang2           #+#    #+#             */
/*   Updated: 2025/10/28 07:19:36 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "libft.h"

// 作用：是否为空白字符。
// 参数：`c`。
// 逻辑：对照空格/制表符等返回 1/0。
int is_space(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

// 作用：计算从 `start_i` 开始的“单词”长度（引号内允许包含空白和符号）。
// 参数：命令串、起点。
// 逻辑：线性前进，遇到分隔符（空白/管道/重定向）停止；遇到 `'`/`"`
// 则调用 `match_quotes` 把整段引号一起计入；若引号未闭合返回负值。
static int calc_word_len(char *str, int start_i)
{
	int j;
	int q_len;

	j = 0;
	while (str[start_i + j] && !(is_token((unsigned char)str[start_i + j])))
	{
		q_len = match_quotes(start_i + j, str, 34);
		if (q_len == -1)
			return (-1);
		j += q_len;
		q_len = match_quotes(start_i + j, str, 39);
		if (q_len == -1)
			return (-1);
		j += q_len;
		if (!str[start_i + j] || is_space(str[start_i + j]))
			break;
		else if (str[start_i + j] == 34 || str[start_i + j] == 39)
			continue;
		else
			j++;
	}
	return (j);
}

// 作用：对提取出的原始子串做**引号处理与属性标记**，并填入 `info`。
// 参数：`substr` 截取的原文；`info` 临时节点信息。
// 逻辑：调用 `remove_quotes_flag`，拿到去壳文本与“出现过单/双引号”的标志，记录到 `info`；
// 失败时清理并返回负值。
static int process_word_data(char *substr, t_token_info *info)
{
	char *clean;

	clean = NULL;
	clean = remove_quotes_flag(substr, &info->had_quotes, &info->quoted_single,
							   &info->quoted_double);
	if (!clean)
	{
		info->raw = NULL;
		info->clean = substr;
		info->had_quotes = 0;
		info->quoted_single = 0;
		info->quoted_double = 0;
	}
	else
	{
		info->raw = substr;
		info->clean = clean;
	}
	return (1);
}

// 作用：根据 `info` 构造 `WORD` 节点并追加到链表。
// 参数：`info`、链表头地址。
// 逻辑：调用 `add_node(info, WORD, list)`；根据返回值判断是否成功，失败回滚需要的内存。
static int finalize_word_node(t_token_info *info, t_lexer **list)
{
	if (!add_node(info, TOK_WORD, list))
	{
		if (info->raw)
			free(info->raw);
		else if (info->clean)
			free(info->clean);
		return (-1);
	}
	return (1);
}

// 作用：在 `str[i]` 解析**一个单词 token**并进链表。
// 参数：命令串、起点、链表头。
// 逻辑：先用 calc_word_len(str, i) 计算从 i 起一个“单词”的长度
// （遇引号用 match_quotes 整段跳过，未闭合返回 -1）；然后 ft_substr 取片段，
// 调用 remove_quotes_flag 去掉外层引号并记录标志，
// 填充 t_token_info，用 add_node(info, WORD, list) 追加到链表；
// 成功返回消费长度，出错清理并返回负值。
int handle_word(char *str, int i, t_lexer **list)
{
	int j;
	char *substr;
	t_token_info info;

	j = 0;
	substr = NULL;
	j = calc_word_len(str, i);
	if (j < 0)
		return (-1);
	if (j == 0)
		return (0);
	substr = ft_substr(str, i, j);
	if (!substr)
		return (-1);
	process_word_data(substr, &info);
	if (finalize_word_node(&info, list) < 0)
		return (-1);
	return (j);
}
