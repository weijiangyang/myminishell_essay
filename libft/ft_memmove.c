/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 07:47:45 by weiyang           #+#    #+#             */
/*   Updated: 2025/05/05 17:16:27 by weijiangyang     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char		*d;
	const unsigned char	*s;

	if (!dst && !src)
		return (NULL);
	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	if (d < s)
	{
		while (len--)
			*d++ = *s++;
	}
	else
	{
		d += len;
		s += len;
		while (len--)
			*--d = *--s;
	}
	return (dst);
}
/*int main(void)
{
    char str[] = "abcdef";

    printf("Before: %s\n", str);

    // Example 1: no overlap
    ft_memmove(str + 2, str, 4);  // Move "1234" into str[2]
    printf("After (overlap forward): %s\n", str);

    // Reset str
    strcpy(str, "abcdef");

    // Example 2: overlap backward
    ft_memmove(str, str + 2, 4);  // Move "34" into str[0]
    printf("After (overlap backward): %s\n", str);

    return 0;
}*/
