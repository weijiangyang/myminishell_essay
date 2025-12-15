#include "../../include/minishell.h"

void sigint_prompt(int sig)
{
    (void)sig;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    g_signal = 1;
}

void setup_prompt_signals(void)
{
    signal(SIGINT, sigint_prompt);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}

void setup_child_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
}

void sigint_breakline(int sig)
{
    (void) sig;
    write(1, "\n", 1);
}

void setup_parent_exec_signals(void)
{
    signal(SIGINT, sigint_breakline);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}

// 处理 SIGINT (Ctrl+C) 信号，专门针对 heredoc
void sigint_heredoc_handler(int sig)
{
    (void)sig; // 防止编译警告

    // 设置全局标志，表示收到 SIGINT 信号
    g_signal = 1;
    write(STDOUT_FILENO, "\n", 1); // 输出换行符，以便用户看到新的提示符
}
