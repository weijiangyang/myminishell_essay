/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_del.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/27 07:19:05 by yzhang2           #+#    #+#             */
/*   Updated: 2025/10/28 07:05:48 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

// 作用：删除链表头结点（并释放资源）。
// 参数：头指针地址。
// 逻辑：调整头指针到 `next`，释放原头（函数体在文件内被截断，但由 `del_one` 使用可知职责）。
void	del_first(t_lexer **lst)
{
	t_lexer	*tmp;

	if (!lst || !*lst)
		return ;
	tmp = *lst;
	*lst = tmp->next;
	clear_one(&tmp);
	if (*lst)
		(*lst)->prev = NULL;
}

// 作用：删除索引为 `target` 的中间节点。
// 参数：起始节点指针、目标索引。
// 逻辑：遍历找到前驱，修改指针并释放目标节点。
static void	remove_target(t_lexer *start, int target)
{
	t_lexer	*prev;
	t_lexer	*node;

	prev = start;
	node = start->next;
	while (node && node->idx != target)
	{
		prev = node;
		node = node->next;
	}
	if (!node)
		return ;
	prev->next = node->next;
	if (node->next)
		node->next->prev = prev;
	clear_one(&node);
}

// 作用：删除链表中**指定 idx** 的节点。
// 参数：头指针地址、目标索引。
// 逻辑：若头结点 `idx==target` → `del_first`；否则调用 `remove_target`。
void	del_one(t_lexer **lst, int target)
{
	t_lexer	*start;

	if (!lst || !*lst)
		return ;
	start = *lst;
	if (start->idx == target)
	{
		del_first(lst);
		return ;
	}
	remove_target(start, target);
}
