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


int handle_heredoc(t_redir *new_redir)
{
    int     pipefd[2];   // 创建管道，pipefd[0] 读端，pipefd[1] 写端
    pid_t   pid;         // fork 后的子进程 PID
    int     status;      // 保存子进程退出状态

    // 1️⃣ 创建管道，用于 heredoc 内容传递
    if (pipe(pipefd) < 0)
        return -1; // pipe 创建失败，返回错误

    // 2️⃣ fork 一个子进程专门处理 heredoc 输入
    pid = fork();
    if (pid < 0)
        return -1; // fork 失败，返回错误

    if (pid == 0)
    {
        /* ===== 子进程：只负责 heredoc ===== */
        close(pipefd[0]);  // 关闭读端，子进程只写

        // 子进程中信号设置：
        signal(SIGINT, SIG_DFL);  // Ctrl+C → 默认行为（中断子进程）
        signal(SIGQUIT, SIG_IGN); // Ctrl+\ → 忽略，不退出子进程

        // 调用 heredoc_loop 处理输入
        // 写入管道写端 pipefd[1]
        if (heredoc_loop(pipefd[1], new_redir->filename) < 0)
            exit(130); // 被 Ctrl+C 打断，退出码 130（bash 风格）

        close(pipefd[1]); // 写入完成，关闭写端
        exit(0);          // 正常完成 heredoc
    }

    /* ===== 父进程 ===== */
    close(pipefd[1]); // 父进程只读，不写

    // 父进程信号设置：
    // heredoc 子进程中按 Ctrl+C 只中断子进程，不影响父进程
    signal(SIGINT, SIG_IGN);  // 忽略 Ctrl+C
    signal(SIGQUIT, SIG_IGN); // 忽略 "Ctrl+\"

    waitpid(pid, &status, 0); // 等待 heredoc 子进程完成

    // 恢复 shell prompt 的信号处理器
    // 比如 Ctrl+C 恢复为只中断当前命令，不退出 shell
    setup_prompt_signals();

    // 检查子进程是否被信号中断
    if (WIFSIGNALED(status))
    {
        close(pipefd[0]); // 关闭管道读端
        return -1;        // 返回错误，表示 heredoc 中断
    }

    // 检查子进程是否因 Ctrl+C 自己退出（exit 130）
    if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
    {
        close(pipefd[0]); // 关闭管道读端
        return -1;        // 返回错误
    }

    // heredoc 正常完成，将管道读端保存到 t_redir 结构
    new_redir->heredoc_fd = pipefd[0];
    return 0; // 成功
}

