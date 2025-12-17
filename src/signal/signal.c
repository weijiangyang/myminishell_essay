#include "../../include/minishell.h"

void sigint_prompt(int sig)
{
    g_signal = sig;
    write(1, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
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
    g_signal = 1;
}

void setup_parent_exec_signals(void)
{
    signal(SIGINT, sigint_breakline);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}


