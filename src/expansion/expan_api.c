/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_api.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:34:25 by yzhang2           #+#    #+#             */
/*   Updated: 2025/11/12 19:09:34 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"


// 做什么：单字符串版本：tmp = expand_all(minishell, str) → clean = remove_quotes_flag(tmp, ...)（若没引号就 ft_strdup(tmp)）→ 释放 tmp 和旧 str → 返回 clean。
// 输入：str（会被函数内部 free）。
// 输出：新堆串或 NULL。
// 谁调：解析重定向目标时可以用（如果没使用 expander_list 统一处理）。
// 调用到的外部函数：expand_all（本模块）、remove_quotes_flag（在 lexer_remove_quotes.c）。
char *expander_str(t_minishell *minishell, char *str)
{
	char *tmp, *clean;
	int had_q, q_s, q_d;

	if (!str)
		return (NULL);
	tmp = expand_all(minishell, str);
	if (!tmp)
		return (NULL);
	clean = remove_quotes_flag(tmp, &had_q, &q_s, &q_d);
	if (!clean)
		clean = ft_strdup(tmp);
	free(tmp);
	if (!clean)
		return (NULL);
	free(str);
	return (clean);
}