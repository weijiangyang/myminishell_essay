#ifndef MINISHELL_H
#define MINISHELL_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

typedef struct s_minishell t_minishell;

#include "../src/lexer/lexer.h"

#include "../src/parse/parse.h"
#include "../src/exec/exec.h"

extern volatile sig_atomic_t g_signal;

typedef struct s_minishell
{
	// lexer
	t_lexer *lexer;
	// char                        *args;

	char *raw_line; // 原始输入行

	int n_pipes; // 管道 “|” 的个数（cmd 数 - 1）

	int last_exit_status; // 上一条命令退出状态（用于 $? 扩展）

	char **envp;
	char **paths;

	// loop
} t_minishell;

#endif

