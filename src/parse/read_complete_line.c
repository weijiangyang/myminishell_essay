#include "../../include/minishell.h"
#include "../../libft/libft.h"

int has_unclosed_quotes(const char *s)
{
    int single = 0;
    int dbl = 0;
    size_t i = 0;

    while (s[i])
    {
        if (s[i] == '\'' && dbl == 0)
            single = !single;
        else if (s[i] == '"' && single == 0)
            dbl = !dbl;
        i++;
    }
    // 若任意一种引号仍处于开启状态，则返回 1（表示未闭合）
    return (single || dbl);
}

char *ft_strjoin_free(char *s1, char *s2, int mode1, int mode2)
{
    char *res;

    res = ft_strjoin(s1, s2);
    if (mode1)
        free(s1);
    if (mode2)
        free(s2);
    return (res);
}

char *read_complete_line(void)
{
    char *line = NULL;
    char *next = NULL;

    line = readline("minishell$> ");
    while (has_unclosed_quotes(line))
    {
        next = readline("> "); // 二级提示符
        if (!next)
            break;
        line = ft_strjoin_free(line, next, 1, 1);
    }
    return (line);
}