/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 09:32:36 by weiyang           #+#    #+#             */
/*   Updated: 2025/05/02 11:01:03 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*ptr;
	char	*dst;

	if (!s1)
		s1 = "";
	if (!s2)
		s2 = "";
	ptr = malloc ((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!ptr)
		return (NULL);
	dst = ptr;
	while (*s1)
	{
		*dst = *s1;
		dst++;
		s1++;
	}
	while (*s2)
	{
		*dst = *s2;
		dst++;
		s2++;
	}
	*dst = '\0';
	return (ptr);
}
