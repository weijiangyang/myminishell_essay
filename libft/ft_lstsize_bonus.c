/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weijiangyang <weijiangyang@laposte.net>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 21:06:17 by weijiangyang      #+#    #+#             */
/*   Updated: 2025/05/18 21:07:00 by weijiangyang     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int	count;

	count = 0;
	while (lst)
	{
		count++;
		lst = lst->next;
	}
	return (count);
}

int ft_lstsize_any(void *lst)
{
    int count = 0;
    void *cur = lst;

    while (cur)
    {
        cur = *(void **)cur; // next 是结构体第一个成员
        count++;
    }
    return count;
}