/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expan_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:33:30 by yzhang2           #+#    #+#             */
/*   Updated: 2025/11/12 19:09:34 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"


// 做什么：从当前 node 开始，在下一个 | 之前找本段第一个 TOK_WORD，检查是否精确等于 "export"。
// 输出：1 是 export 段 / 0 否。
// 谁调：expander_list，用于决定本段 TOK_WORD 是否要保留引号。
/*static int	is_export_segment(t_lexer *node)
{
	t_lexer	*p;

	p = node;
	while (p && p->tokentype != TOK_PIPE)
	{
		if (p->tokentype == TOK_WORD && p->str && p->str[0])
		{
			if (p->str[0] == 'e' && p->str[1] == 'x' && p->str[2] == 'p'
				&& p->str[3] == 'o' && p->str[4] == 'r' && p->str[5] == 't'
				&& p->str[6] == '\0')
				return (1);
			return (0);
		}
		p = p->next;
	}
	return (0);
}*/
static int is_export_segment(t_lexer *node)//修改： 不需要遍历
{
    // 确保节点有效并且是一个单词类型的 token
    if (node && node->tokentype == TOK_WORD && node->str && node->str[0])
    {
        // 检查该单词是否是 "export"
        if (node->str[0] == 'e' && node->str[1] == 'x' && node->str[2] == 'p'
            && node->str[3] == 'o' && node->str[4] == 'r' && node->str[5] == 't'
            && node->str[6] == '\0')
        {
            return 1;  // 如果是 "export"，返回 1
        }
    }
    return 0;  // 否则，返回 0
}


// 做什么：按管道段遍历整条链表：
// 每段先 export_mode = is_export_segment(p)；
// 在该段内：对每个 token 调 expand_token(minishell, node, export_mode)；
// 遇到 TOK_PIPE 切到下一段。
// 输入：minishell、链表头 head。
// 输出：1 成功 / 0 失败（任一 expand_token 失败）。
// 谁调：词法结束后、解析/执行前的主流程里调用一次。
int	expander_list(t_minishell *minishell, t_lexer *head)
{
	t_lexer	*p;
	int		export_mode;

	p = head;
	while (p)
	{
		export_mode = is_export_segment(p);
		while (p && p->tokentype != TOK_PIPE)
		{
			if (!expand_token(minishell, p, export_mode))
				return (0);
			p = p->next;
		}
		if (p && p->tokentype == TOK_PIPE)
			p = p->next;
	}
	return (1);
}
