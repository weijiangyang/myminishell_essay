/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:28:34 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:28:36 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

    line = readline("minishell$ >");
    while (has_unclosed_quotes(line))
    {
        next = readline("> "); // 二级提示符
        if (!next)
            break;
        line = ft_strjoin_free(line, next, 1, 1);
    }
    return (line);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    char *buf;
    t_minishell *general;

    while (1)
    {
        buf = read_complete_line(); // ✅ 提示符
        if (!buf)
        {
            printf("exit\n");
            break; // EOF（Ctrl+D）
        }

        if (*buf == '\0')
        {
            free(buf);
            continue; // 空输入，直接重新读取
        }

        add_history(buf); // 保存历史

        general = calloc(1, sizeof(t_minishell));
        if (!general)
        {
            perror("calloc");
            free(buf);
            break;
        }
        general->raw_line = buf;

        // === Lexer 阶段 ===
        if (handle_lexer(general))
        {
            printf("Lexer tokens:\n");
            print_lexer(general->lexer);
        }
        if (!general->lexer)
        {
            fprintf(stderr, "tokenize failed\n");
            free(buf);
            free(general);
            continue;
        }

        // === Parser 阶段 ===
        t_lexer *cursor = general->lexer;
        ast *root = parse_cmdline(&cursor);
        if (root)
        {
            printf("=== AST ===\n");
            print_ast(root, 0);
            exec_ast(root);
            free_ast(root);
        }
        else
        {
            fprintf(stderr, "Parsing failed.\n");
        }

        // === 清理内存 ===
        free_tokens(general->lexer);
        free(buf);
        free(general);
    }

    rl_clear_history();
    return 0;
}
