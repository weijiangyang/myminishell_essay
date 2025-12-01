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
/**
 * heredoc_loop
 * ------------------------------------------------------------
 * 目的：
 *   处理 heredoc 输入循环，读取标准输入直到遇到指定 delimiter。
 *   将每一行写入给定的文件描述符。
 *
 * 参数：
 *   @write_fd  — 用于写入 heredoc 内容的文件描述符。
 *   @delimiter — 终止输入的字符串（不包含换行）。
 *
 * 返回值：
 *   0   — 成功完成 heredoc。
 *   -1  — 写入失败或其他错误（同时打印 perror）。
 *
 * 逻辑：
 *   1. 循环提示用户输入 "heredoc< "。
 *   2. 使用 getline 读取整行输入。
 *   3. 移除行尾换行符。
 *   4. 如果输入等于 delimiter，则跳出循环。
 *   5. 否则将原始输入（带换行）写入 write_fd。
 *   6. 循环直到遇到 delimiter 或输入错误。
 *   7. 释放 getline 分配的缓冲区。
 *
 * 特性：
 *   - 安全处理空行和 EOF。
 *   - 避免 double write / 未关闭缓冲区。
 *   - perror 会在写入错误时打印信息。
 */
/*int heredoc_loop(int write_fd, const char *delimiter)
{
    char *line;
    size_t len;
    ssize_t nread;

    line = NULL;
    len = 0;
    while (1)
    {
        write(1, "heredoc< ", 9);
        nread = getline(&line, &len, stdin);
        if (nread < 0)
            break;
        if (nread > 0 && line[nread - 1] == '\n')
            line[nread - 1] = '\0';
        if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0)
            break;
        line[nread - 1] = '\n';
        if (write(write_fd, line, nread) < 0)
            return (perror("write"), free(line), -1);
    }
    free(line);
    return 0;
}*/

int heredoc_loop(int write_fd, const char *delimiter)
{
    char *line;
    size_t len;

    while (1)
    {
        write(1, "heredoc< ", 9);
        line = get_next_line(STDIN_FILENO);
        if (!line)
            break;
        len = ft_strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';
        if (ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0)
            return (free(line), 0);
        if (len > 0)
            line[len - 1] = '\n';
        if (write(write_fd, line, len) < 0)
            return (perror("write"), free(line), -1);
        free(line);
    }
    return 0;
}

/**
 * handle_heredoc
 * ------------------------------------------------------------
 * 目的：
 *   创建一个管道用于 heredoc 输入，并调用 heredoc_loop 将用户输入写入管道。
 *   将管道的读端保存到 t_redir 结构，供后续命令执行阶段使用。
 *
 * 参数：
 *   @new_redir — 指向 t_redir 节点，该节点包含 heredoc 的 delimiter。
 *
 * 返回值：
 *   0   — 成功创建 heredoc 管道并写入内容。
 *   -1  — 管道创建失败或写入失败。
 *
 * 逻辑：
 *   1. 使用 pipe() 创建一个管道 pipefd[2]。
 *      - pipefd[1] 用于写入 heredoc 内容。
 *      - pipefd[0] 用于后续命令读取。
 *   2. 调用 heredoc_loop(pipefd[1], new_redir->filename) 处理输入。
 *      - 如果失败，关闭管道两端并返回 -1。
 *   3. 关闭写端 pipefd[1]，保留读端 pipefd[0]。
 *   4. 将读端 fd 保存到 new_redir->heredoc_fd。
 *
 * 特性：
 *   - 避免 heredoc 写入失败导致资源泄漏。
 *   - 读端 fd 保留供执行阶段使用，写端及时关闭。
 *   - 与 heredoc_loop 协作，保证用户输入安全写入管道。
 */
int handle_heredoc(t_redir *new_redir)
{
    int pipefd[2];

    if (pipe(pipefd) < 0)
        return -1;
    if (heredoc_loop(pipefd[1], new_redir->filename) < 0)
        return (close(pipefd[0]), close(pipefd[1]), -1);
    close(pipefd[1]);                  /* 关闭写端 */
    new_redir->heredoc_fd = pipefd[0]; /* 读端保存在 t_redir */
    return 0;
}
