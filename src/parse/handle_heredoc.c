#include "../../include/minishell.h"


volatile sig_atomic_t g_signal; // 唯一全局变量

/* SIGINT handler */
void sigint_heredoc(int sig)
{
    (void)sig;
    g_signal = SIGINT;
}

int heredoc_loop(int write_fd, const char *delimiter)
{
    char *line;
    struct sigaction sa;

    sa.sa_handler = sigint_heredoc;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    while (1)
    {
        write(STDOUT_FILENO, "heredoc> ", 9);
        g_signal = 0;
        line = get_next_line(STDIN_FILENO);

        if (g_signal == SIGINT)
        {
            free(line);
            write(1, "\n", 1);
            return -1; // Ctrl+C 中断
        }

        if (!line) break; // Ctrl+D

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        if (strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }

        write(write_fd, line, strlen(line));
        write(write_fd, "\n", 1);
        fsync(write_fd); // 保证立即可读
        free(line);
    }
    return 0;
}





int handle_heredoc(t_redir *new_redir, t_minishell *shell)
{
    int pipefd[2];
    pid_t pid;
    int status;

    if (pipe(pipefd) < 0) return -1;

    pid = fork();
    if (pid < 0) return -1;

    if (pid == 0)
    {
        close(pipefd[0]);
        if (heredoc_loop(pipefd[1], new_redir->filename) < 0)
            exit(130);
        close(pipefd[1]);
        exit(0);
    }

    close(pipefd[1]);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
    {
        close(pipefd[0]);
        new_redir->heredoc_fd = -1;
        shell->last_exit_status = 130;
        return 1;
    }

    new_redir->heredoc_fd = pipefd[0];
    return 0;
}
