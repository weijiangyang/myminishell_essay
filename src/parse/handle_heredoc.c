#include "../../include/minishell.h"
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t g_signal;

static void sigint_heredoc(int sig)
{
    (void)sig;
    g_signal = 1;
    write(STDOUT_FILENO, "\n", 1);
}

int heredoc_loop(int write_fd, const char *delimiter)
{
    char *line;

    g_signal = 0;
    
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigint_heredoc;
    sa.sa_flags = 0; // 关键：不设置 SA_RESTART
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &(struct sigaction){.sa_handler = SIG_IGN}, NULL);
    sigaction(SIGTSTP, &(struct sigaction){.sa_handler = SIG_IGN}, NULL);
    while (!g_signal)
    {
        write(STDOUT_FILENO, "heredoc> ", 9);
        line = get_next_line(STDIN_FILENO);
        if (!line)
        {
            if(g_signal)              
                break;//信号中断返回 NULL
            write(1, "\n", 1);
            printf("bash: warning: here-document delimited by end-of-file(wanted '%s')\n", delimiter);
            break; // EOF
        }   
        size_t len = ft_strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';
        if (strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }
        write(write_fd, line, ft_strlen(line));
        write(write_fd, "\n", 1);
        free(line);
    }
    if (g_signal)
           return -1; 
    else
        return 0;
}

int handle_heredoc(t_redir *new_redir, t_minishell *shell)
{
    int pipefd[2];
    pid_t pid;
    int status;

    if (pipe(pipefd) < 0)
        return -1;

    pid = fork();
    if (pid < 0)
        return -1;

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
    
    if ((WIFEXITED(status) && WEXITSTATUS(status) == 130) || WIFSIGNALED(status))
    {
        close(pipefd[0]);
        new_redir->heredoc_fd = -1;
        shell->last_exit_status = 130;
        return 1;
    }
    new_redir->heredoc_fd = pipefd[0];
    return 0;
}
