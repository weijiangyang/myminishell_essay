/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 08:15:57 by weiyang           #+#    #+#             */
/*   Updated: 2025/05/02 11:02:36 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	len_number(int n)
{
	int		count;
	long	nb;

	nb = n;
	count = 1;
	if (nb < 0)
	{
		nb = -nb;
		count++;
	}
	while (nb / 10 > 0)
	{
		count++;
		nb /= 10;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	char	*str;
	int		i;
	long	nb;

	nb = (long)n;
	i = len_number(nb) - 1;
	str = (char *)malloc (sizeof (char) * (len_number(nb) + 1));
	if (!str)
		return (NULL);
	str[i + 1] = '\0';
	if (nb < 0)
	{
		str[0] = '-';
		nb = -nb;
	}
	if (nb == 0)
		str[0] = '0';
	while (nb > 0)
	{
		str[i] = nb % 10 + '0';
		i--;
		nb /= 10;
	}
	return (str);
}
/*
#include <stdio.h>

int	main(void)
{
	int	n;
	int	count;
	char	*str;

	n = -4; 
	count = len_number(n);
	str = ft_itoa(n);
	printf("the result is %s", str);
	return (0);
}*/
