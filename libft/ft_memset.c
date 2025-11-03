/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/25 10:42:15 by weiyang           #+#    #+#             */
/*   Updated: 2025/05/02 10:39:22 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h" 

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*mem;
	size_t			i;

	i = 0;
	mem = (unsigned char *)s;
	while (i < n)
	{
		mem[i] = (unsigned char)c;
		i++;
	}
	return (s);
}

/*#include <stdio.h>

int	main(void)
{
	char	arr[10];
	int		c;
	size_t		n;
	size_t		i;

	i = 1;
	c = 67;
	n = 9;
	memset(arr, c,n);
	while (i < n)
	{
		printf("%c", arr[i]);
		i++;
	}
	return (0);
}*/	
