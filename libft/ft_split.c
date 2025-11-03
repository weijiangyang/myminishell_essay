/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 10:24:55 by weiyang           #+#    #+#             */
/*   Updated: 2025/05/18 22:14:08 by weijiangyang     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	count_words(char const *s, char c)
{
	int	count;
	int	in_word;

	in_word = 0;
	count = 0;
	while (*s)
	{
		if (*s != c && in_word == 0)
		{
			in_word = 1;
			count ++;
		}
		else if (*s == c)
			in_word = 0;
		s++;
	}
	return (count);
}

int	len_word(char const *s, char c)
{
	int	len;

	len = 0;
	while (*s && *s != c)
	{
		len++;
		s++;
	}
	return (len);
}

char	*write_word(char const *s, char c)
{
	int		len;
	char	*word;
	int		i;

	i = 0;
	len = len_word(s, c);
	word = (char *)malloc ((len + 1) * sizeof(char));
	if (!word)
		return (NULL);
	while (s[i] && s[i] != c)
	{
		word[i] = s[i];
		i++;
	}
	word[i] = '\0';
	return (word);
}

char	**ft_split_1(char **arr, char const *s, char c, int nbr_words)
{
	int		i;

	i = 0;
	while (i < nbr_words)
	{
		while (*s == c)
			s++;
		if (*s != c && *s)
		{
			arr[i] = write_word(s, c);
			if (!arr[i])
			{
				while (i >= 0)
					free(arr[i--]);
				free (arr);
				return (NULL);
			}
			s += len_word(s, c);
			i++;
		}
	}
	arr[i] = NULL;
	return (arr);
}

char	**ft_split(char const *s, char c)
{
	char	**arr;
	int		nbr_words;

	if (!s)
		return (NULL);
	nbr_words = count_words(s, c);
	arr = (char **)malloc ((nbr_words + 1) * sizeof (char *));
	if (!arr)
		return (NULL);
	return (ft_split_1(arr, s, c, nbr_words));
}
/*#include <stdio.h>

int	main(void)
{
	char const *s = "ab,cd,ef, g";
	int	count = count_words(s,',');
	char **result;
	int	i;
	int	nbr_words;

	nbr_words = count_words(s, ',');
	i = 0;
	result = ft_split(s, ',');
	while (i < nbr_words)
	{
		printf("%s\n", result[i]);
		i++;
	}
	return (0);
}*/	
