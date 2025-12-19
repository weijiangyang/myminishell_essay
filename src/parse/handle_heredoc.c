#include "../../include/minishell.h"


volatile sig_atomic_t g_signal; // 唯一全局变量

/* SIGINT handler */
void sigint_heredoc(int sig)
{
    (void)sig;
    g_signal = SIGINT;
}

/* heredoc_loop: canonical 模式，和 bash 行为一致 */
int heredoc_loop(int write_fd, const char *delimiter)
{
    char *line;
    char *full_line = NULL; // 用于拼接没有换行符的片段

    struct sigaction sa = {.sa_handler = sigint_heredoc, .sa_flags = 0};
    sigaction(SIGINT, &sa, NULL);

    while (1)
    {
        // 只有当缓冲区为空时，才打印提示符
        if (full_line == NULL)
            write(STDOUT_FILENO, "heredoc> ", 9);

        g_signal = 0;
        line = get_next_line(STDIN_FILENO);

        // --- 处理 Ctrl+C ---
        if (g_signal == SIGINT) {
            free(line); 
            free(full_line);
            write(1, "\n", 1);
            return -1;
        }

        // --- 处理 Ctrl+D ---
        if (!line) {
            if (full_line == NULL) { // 纯空行按 Ctrl+D
                printf("bash: warning: ... (wanted '%s')\n", delimiter);
                break;
            }
            // 如果有残留内容按 Ctrl+D，Bash 会将其视为一行处理
            line = full_line;
            full_line = NULL;
        } 
        else if (full_line) { // 如果之前有没写完的片段，拼接起来
            char *tmp = ft_strjoin(full_line, line);
            free(full_line); free(line);
            line = tmp;
        }

        // --- 判断是否读到了完整的一行 ---
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0'; // 去掉换行符
            if (strcmp(line, delimiter) == 0) {
                free(line);
                break;
            }
            write(write_fd, line, strlen(line));
            write(write_fd, "\n", 1);
            free(line);
            full_line = NULL; // 清空暂存，下次循环会打印提示符
        } else {
            // 没有换行符，说明用户按了 Ctrl+D
            // 我们暂存这段文字，进入下一次 read，不打印提示符
            full_line = line;
        }
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
