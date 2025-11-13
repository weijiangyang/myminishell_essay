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

#include "include/minishell.h"
#include "libft/libft.h"

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