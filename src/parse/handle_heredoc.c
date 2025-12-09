/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 16:25:32 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/25 16:25:36 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

volatile sig_atomic_t g_signal = 0;

/* SIGINT 处理函数 */
static void heredoc_sigint_handler(int signo)
{
    (void)signo;
    g_signal = 1;
    write(1, "\n", 1); // 打印换行
}

/* heredoc 输入循环（安全处理 Ctrl+C） */
int heredoc_loop(int write_fd, const char *delimiter)
{
    char *line;
    size_t len;

    /* 保存旧的 SIGINT handler */
    struct sigaction old_sa, new_sa;
    new_sa.sa_handler = heredoc_sigint_handler;
    sigemptyset(&new_sa.sa_mask);
    new_sa.sa_flags = 0;
    sigaction(SIGINT, &new_sa, &old_sa);

    g_signal = 0;

    while (!g_signal)
    {
        write(1, "heredoc< ", 9);
        line = get_next_line(STDIN_FILENO);
        if (!line)
            break;

        len = ft_strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0)
        {
            free(line);
            break;
        }

        if (len > 0)
            line[len - 1] = '\n';

        if (write(write_fd, line, len) < 0)
        {
            perror("write");
            free(line);
            break;
        }
        free(line);
    }

    /* 恢复旧的 SIGINT handler */
    sigaction(SIGINT, &old_sa, NULL);

    if (g_signal)
        return -1;

    return 0;
}

/* 处理 heredoc 管道 */
int handle_heredoc(t_redir *new_redir)
{
    int pipefd[2];

    if (pipe(pipefd) < 0)
        return -1;

    if (heredoc_loop(pipefd[1], new_redir->filename) < 0)
    {
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    close(pipefd[1]);
    new_redir->heredoc_fd = pipefd[0];
    return 0;
}
