#include "../../include/minishell.h"


int heredoc_loop(int write_fd, const char *delimiter)
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
        if (ft_strncmp(line, delimiter, nread - 1) == 0)
            break;
        line[nread - 1] = '\n';
        if (write(write_fd, line, nread) < 0)
            return (perror("write"), free(line), -1);
    }
    free(line);
    return 0;
}


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