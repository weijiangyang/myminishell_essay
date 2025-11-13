/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_handle.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 07:53:04 by yzhang2           #+#    #+#             */
/*   Updated: 2025/10/28 07:06:18 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"


// 作用：跳过从 `i` 开始的连续空白。
// 参数：命令串、起点。

volatile sig_atomic_t g_signal = 0;
int	skip_spaces(char *str, int i)
{
	int	j;

	j = 0;
	while (is_space(str[i + j]))
		j++;
	return (j);
}

// 作用：处理中断：若处于 SIGINT 状态，清空已构建的词法链表并返回“被打断”。
// 参数：链表头地址。
// 逻辑：检测全局信号（你项目）；如触发→`clear_list(list)`→复位→返回 1，否则 0。
static int	check_sigint(t_lexer **list)
{
	if (g_signal == SIGINT)
	{
		clear_list(list);
		g_signal = 0;
		return (1);
	}
	return (0);
}

// 作用：对 `general->args` 执行整行词法拆分。
// 参数：全局上下文（含输入字符串 `args` 与输出链表 `lexer`）。
// 实现逻辑：
//   * 初始化索引 `i`，循环直到 `args[i]=='\0'`；
//   * 先 `skip_spaces`；
//   * 若 `is_token(args[i])` 为真 → `j = handle_token(...)`；
// 否则 `j = handle_word(...)`；
//   * 若 `j < 0`（如引号错误/内存失败）→ `clear_list(&general->lexer)` 并返回 `0`（失败）；
//   * 否则 `i += j` 继续；
//   * 结束返回 `1`（成功）。
int	handle_lexer(t_minishell *general)
{
	int	i;
	int	j;

	if (!general || !general->raw_line)
		return (0);
	i = 0;
	while (general->raw_line[i])
	{
		if (check_sigint(&general->lexer))
			return (0);
		i += skip_spaces(general->raw_line, i);
		if (general->raw_line[i] == '\0')
			break ;
		if (is_token((unsigned char)general->raw_line[i]))
			j = handle_token(general->raw_line, i, &general->lexer);
		else
			j = handle_word(general->raw_line, i, &general->lexer);
		if (j < 0)
		{
			clear_list(&general->lexer);
			return (0);
		}
		i += j;
	}
	
	add_node(NULL, TOK_END, &general->lexer);//把一个 “结束标记 token” (TOK_END) 添加到 general->lexer 链表末尾
	return (1);
}


