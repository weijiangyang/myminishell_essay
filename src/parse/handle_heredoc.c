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

volatile sig_atomic_t g_signal;

int heredoc_loop(int write_fd, const char *delimiter)
{
    char *line;

    g_signal = 0; // 初始化信号标志

    while (!g_signal) // 如果用户按 Ctrl+C，会由信号 handler 设置 g_signal = 1
    {
        write(1, "heredoc< ", 9);

        line = get_next_line(STDIN_FILENO);
        if (!line)
            break;

        // 去掉末尾换行
        size_t len = ft_strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        // 遇到 delimiter，结束 heredoc
        if (strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }

        // 写入管道
        write(write_fd, line, len);
        free(line);
    }

    // 如果被 Ctrl+C 打断，返回 -1
    if (g_signal)
        return -1;

    return 0;
}


int handle_heredoc(t_redir *new_redir, t_minishell *shell)
{
    int     pipefd[2];
    pid_t   pid;
    int     status;

    if (pipe(pipefd) < 0)
        return -1;

    pid = fork();
    if (pid < 0)
        return -1;

    if (pid == 0)
    {
        close(pipefd[0]); // 子进程只写

        // 子进程信号处理
        signal(SIGINT, SIG_DFL);  // Ctrl+C 中断子进程
        signal(SIGQUIT, SIG_IGN);

        if (heredoc_loop(pipefd[1], new_redir->filename) < 0)
            exit(130); // Ctrl+C 中断

        close(pipefd[1]);
        exit(0);
    }

    // 父进程
    close(pipefd[1]);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    waitpid(pid, &status, 0);

    // 子进程被 Ctrl+C 中断
    if ((WIFEXITED(status) && WEXITSTATUS(status) == 130) || WIFSIGNALED(status))
    {
        close(pipefd[0]);
        new_redir->heredoc_fd = -1;       // 标记中断
        shell->last_exit_status = 130;    // 父进程保存退出码
        return -1;                        // 上层可检测
    }

    new_redir->heredoc_fd = pipefd[0];   // 正常完成
    return 0;
}
