/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:42:25 by weiyang           #+#    #+#             */
/*   Updated: 2025/05/28 09:57:55 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	end_line(char *str)
{
	while (*str)
	{
		if (*str == '\n')
			return (1);
		str++;
	}
	return (0);
}

char	*extract_line(char *str)
{
	int		i;
	char	*extract;
	int		j;

	j = 0;
	i = 0;
	if (!str || str[0] == '\0')
		return (NULL);
	while (str[i] && str[i] != '\n')
		i++;
	if (str[i] == '\n')
		i++;
	extract = (char *) malloc ((i + 1) * sizeof (char));
	if (!extract)
		return (NULL);
	while (j < i)
	{
		extract[j] = str[j];
		j++;
	}
	extract[j] = '\0';
	return (extract);
}
