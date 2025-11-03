/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weijiangyang <weijiangyang@laposte.net>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 21:07:32 by weijiangyang      #+#    #+#             */
/*   Updated: 2025/05/19 08:55:08 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*new_node;

	new_node = (t_list *)malloc(sizeof(t_list));
	if (!new_node)
		return (NULL);
	new_node->content = content;
	new_node->next = NULL;
	return (new_node);
}

/*#include <stdio.h>

int main(void)
{
    t_list *node = ft_lstnew("hello, i'm a data");

    if (!node)
    {
        printf("Erreur d'allocation\n");
        return (1);
    }

    printf("Contenu : %s\n", (char *)node->content);
    return (0);
}
*/
