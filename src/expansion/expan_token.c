/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:32:00 by yzhang2           #+#    #+#             */
/*   Updated: 2025/11/12 19:09:34 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/minishell.h"


// 做什么：调用 remove_quotes_flag(s, &had_q, &q_s, &q_d) 去引号；如果本来没引号，返回 ft_strdup(s)。
// 输入：原串 s。
// 输出：新堆串（去壳后或复制品）。
// 在哪调：仅本文件 handle_strip_quotes。
static char	*strip_all_quotes_dup(const char *s, int *had_q, int *q_s, int *q_d)
{
	char	*clean;

	clean = remove_quotes_flag(s, had_q, q_s, q_d);
	if (!clean)
		return (ft_strdup(s));
	return (clean);
}


// 做什么：对 expanded 去引号，写回 n->str，维护 n->had_quotes / n->quoted_by，并释放 n->raw。
// 输入：词法节点 n、已展开的新串 expanded。
// 输出：1 成功 / 0 失败（内存）。
// 在哪调：expand_token 里非 export 的 TOK_WORD 和所有重定向 token 情况。
static int	handle_strip_quotes(t_lexer *n, char *expanded)
{
	char	*clean;
	int		had_q;
	int		q_s;
	int		q_d;

	clean = strip_all_quotes_dup(expanded, &had_q, &q_s, &q_d);
	if (!clean)
	{
		free(expanded);
		return (0);
	}
	free(n->str);
	n->str = clean;
	n->had_quotes = had_q;
	n->quoted_by = q_s ? '\'' : (q_d ? '\"' : 0);
	if (n->raw)
	{
		free(n->raw);
		n->raw = NULL;
	}
	free(expanded);
	return (1);
}

// 做什么：保留引号地写回 n->str，并释放 n->raw（export 段需求）。
// 输出：1。
// 在哪调：expand_token（export 段的 TOK_WORD）。
static int	handle_keep_quotes(t_lexer *n, char *expanded)
{
	free(n->str);
	n->str = expanded;
	n->had_quotes = 0;
	n->quoted_by = 0;
	if (n->raw)
	{
		free(n->raw);
		n->raw = NULL;
	}
	return (1);
}

// 做什么（核心）：对一个 token执行：
// 选源串：优先 n->raw（含引号），否则 n->str；
// expanded = expand_all(msh, src)（只展开 $，不去引号）；
// 决策：
// 若 TOK_WORD 且 export_mode == 0 → 去引号：handle_strip_quotes；
// 若 TOK_WORD 且 export_mode == 1 → 保留引号：handle_keep_quotes；
// 若重定向 token → 去引号：handle_strip_quotes；
// 其它 token（如管道）→ 直接 free(expanded) 不改。
// 输入：msh，节点 n，当前管道段是否 export 模式。
// 输出：1/0。
// 谁调：expander_list。
int	expand_token(t_minishell *msh, t_lexer *n, int export_mode)
{
	char	*src;
	char	*expanded;

	src = (n->raw && n->raw[0]) ? n->raw : n->str;
	if (!src)
		return (1);
	expanded = expand_all(msh, src);
	if (!expanded)
		return (0);
	if ((n->tokentype == TOK_WORD && !export_mode)
		|| is_redir_token(n))
	{
		return (handle_strip_quotes(n, expanded));
	}
	if (n->tokentype == TOK_WORD && export_mode)
	{
		return (handle_keep_quotes(n, expanded));
	}
	free(expanded);
	return (1);
}
