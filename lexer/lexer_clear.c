/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_clear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 18:03:40 by yzhang2           #+#    #+#             */
/*   Updated: 2025/10/28 07:02:29 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

// 作用：释放一个词法节点内部动态资源。
// 参数：节点指针。
// 逻辑：逐字段判空 `free`，再置空以防悬垂。（函数体被截断，但与同文件其他函数配合可确定职责）
static void	free_lexer_content(t_lexer *node)
{
	if (!node)
		return ;
	if (node->raw && node->raw == node->str)
	{
		free(node->raw);
		node->raw = NULL;
		node->str = NULL;
	}
	else
	{
		if (node->raw)
		{
			free(node->raw);
			node->raw = NULL;
		}
		if (node->str)
		{
			free(node->str);
			node->str = NULL;
		}
	}
}

// 作用：释放并删除当前指向的单个词法节点（含其内部动态内存），将头指针置为 NULL。
// 参数/逻辑：入参是链表头指针地址；判空后取出节点→free_lexer_content
// →断开 next/prev→free(node)→*lst=NULL→返回 NULL。
t_lexer	*clear_one(t_lexer **lst)
{
	t_lexer	*node;

	if (!lst || !*lst)
		return (NULL);
	node = *lst;
	free_lexer_content(node);
	node->next = NULL;
	node->prev = NULL;
	free(node);
	*lst = NULL;
	return (NULL);
}

// 作用：清空并释放整个词法链表。
// 参数：头指针地址。
// 逻辑：遍历：缓存 `next` → `clear_one(lst)`（你项目中的单节点释放器）→前进；
// 结束后头指针为 `NULL`。
void	clear_list(t_lexer **lst)
{
	t_lexer	*tmp;

	if (!lst || !*lst)
		return ;
	while (*lst)
	{
		tmp = (*lst)->next;
		clear_one(lst);
		*lst = tmp;
	}
}
