/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weijiangyang <weijiangyang@laposte.net>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 20:55:51 by weijiangyang      #+#    #+#             */
/*   Updated: 2025/05/18 20:57:32 by weijiangyang     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (!lst || !new)
		return ;
	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	last = *lst;
	while (last->next)
		last = last->next;
	last->next = new;
}

void ft_lstadd_back_any(void **lst, void *new_node)
{
    void *cur;

    if (!lst || !new_node)
        return;

    if (*lst == NULL) {
        *lst = new_node;
        return;
    }

    cur = *lst;

    // 把每个节点当成 “第一个成员是 next 的结构体”
    while ( *(void **)cur != NULL )
        cur = *(void **)cur;

    *(void **)cur = new_node;
}
