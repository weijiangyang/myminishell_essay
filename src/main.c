/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:28:34 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:28:36 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "../libft/libft.h"

/**
 * has_unclosed_quotes
 * ----------------
 * 目的：
 *   检查字符串中是否存在未闭合的单引号或双引号。
 *
 * 参数：
 *   - s : 待检查的字符串
 *
 * 返回值：
 *   - 1 : 存在未闭合的引号
 *   - 0 : 所有引号均已闭合
 *
 * 行为说明：
 *   1. 使用两个标志变量 single 和 dbl 分别跟踪单引号和双引号的开闭状态
 *   2. 遍历字符串：
 *        - 遇到单引号且不在双引号内部，切换 single 状态
 *        - 遇到双引号且不在单引号内部，切换 dbl 状态
 *   3. 遍历结束后，如果任一标志为 1，说明有未闭合的引号
 */
static int has_unclosed_quotes(const char *s)
{
    int single = 0;
    int dbl = 0;
    size_t i = 0;

    while (s[i])
    {
        if (s[i] == '\'' && dbl == 0)
            single = !single;
        else if (s[i] == '"' && single == 0)
            dbl = !dbl;
        i++;
    }
    return (single || dbl);
}

/**
 * ft_strjoin_free
 * ----------------
 * 目的：
 *   将两个字符串连接成一个新字符串，并根据参数选择释放原字符串。
 *
 * 参数：
 *   - s1    : 第一个字符串
 *   - s2    : 第二个字符串
 *   - mode1 : 如果非 0，连接后释放 s1
 *   - mode2 : 如果非 0，连接后释放 s2
 *
 * 返回值：
 *   - 返回新连接的字符串指针
 *
 * 行为说明：
 *   1. 调用 ft_strjoin 将 s1 和 s2 连接成新字符串
 *   2. 根据 mode1 和 mode2 决定是否释放 s1 或 s2
 *   3. 返回新字符串指针
 */
static char *ft_strjoin_free(char *s1, char *s2, int mode1, int mode2)
{
    char *res;

    res = ft_strjoin(s1, s2);
    if (mode1)
        free(s1);
    if (mode2)
        free(s2);
    return (res);
}

/*
 * 函数名: get_relative_path
 * -----------------------------------------------------------------------------
 * 功能: 将当前工作目录 (cwd) 的绝对路径转换为相对于用户主目录 (~/) 的相对路径。
 * 如果 cwd 在 $HOME 之下，则用 '~' 替换 $HOME 的部分。
 * * 参数:
 * cwd: 当前工作目录的绝对路径字符串 (const char *，通常来自 getcwd 的结果)。
 * * 返回值:
 * - 成功: 新分配的字符串指针，包含相对路径（例如 "~/minishell"）。
 * - 失败: 如果内存分配失败或 $HOME 无效，返回原始绝对路径的副本，或返回 "."。
 * * 注意:
 * - 调用者必须使用 free() 释放此函数返回的指针，以避免内存泄漏。
 */
static char *get_relative_path(const char *cwd)
{
    const char *home_dir = getenv("HOME");
    char *relative_path;
    size_t home_len;

    if (!cwd || !home_dir)
        return strdup(cwd ? cwd : ".");
    home_len = strlen(home_dir);
    if (strncmp(cwd, home_dir, home_len) == 0 && (cwd[home_len] == '/' || cwd[home_len] == '\0'))
    {
        size_t suffix_len = strlen(cwd + home_len);
        relative_path = (char *)malloc(1 + suffix_len + 1);
        if (!relative_path)
            return strdup(cwd);
        relative_path[0] = '~';
        strcpy(relative_path + 1, cwd + home_len);
        return relative_path;
    }
    return strdup(cwd);
}

/*
 * 函数名: read_complete_line
 * -----------------------------------------------------------------------------
 * 功能:
 * 1. 获取当前工作目录 (CWD)，并转换为相对于 $HOME 的相对路径（例如：~）。
 * 2. 使用 get_relative_path 的结果和 "$ " 常量生成完整的 readline 提示符。
 * 3. 使用 readline() 获取用户输入。
 * 4. 检查输入是否包含未闭合的引号 ('has_unclosed_quotes' 假定实现)，如果包含，
 * 则循环使用 "> " 提示符读取后续行，直到引号闭合或用户按下 EOF (Ctrl+D)。
 * 5. 使用 ft_strjoin_free 拼接多行输入。
 * * 返回值:
 * - 成功: 用户输入的完整、拼接后的行字符串 (char *，由 readline 或 ft_strjoin_free 分配)。
 * - 失败: 如果内存分配失败或用户输入 EOF (Ctrl+D)，返回 NULL。
 * * 注意:
 * - 必须释放为提示符分配的内存 (full_prompt 和 relative_path)。
 * - 调用者必须负责 free() 返回的 'line' 指针。
 */
static char *read_complete_line(void)
{
    char *line;
    char *next;
    char cpth[1000];
    char *relative_path;
    char *full_prompt;

    getcwd(cpth, 4096);
    relative_path = get_relative_path(cpth);
    full_prompt = ft_strjoin(relative_path, "$ ");
    free(relative_path);
    if (!full_prompt)
        return (NULL);
    line = readline(full_prompt);
    if (!line)
        return NULL;
    while (has_unclosed_quotes(line))
    {
        next = readline("> ");
        if (!next)
            break;
        line = ft_strjoin_free(line, next, 1, 1);
    }
    free(full_prompt);
    return (line);
}

/**
 * main
 * ----------------
 * 目的：
 *   Minishell 主函数：循环读取用户输入，进行词法分析、解析、执行，
 *   并在每次循环结束后释放相关资源。
 *
 * 参数：
 *   - argc : 命令行参数数量（未使用）
 *   - argv : 命令行参数数组（未使用）
 *
 * 返回值：
 *   - 返回 0 表示正常退出
 *
 * 行为说明：
 *   1. 无限循环读取用户输入
 *   2. 调用 read_complete_line 获取完整命令行（支持多行未闭合引号）
 *   3. 如果输入为 NULL（用户中断或 EOF），打印 "exit" 并退出循环
 *   4. 忽略空行，添加非空行到历史记录
 *   5. 分配 t_minishell 结构存储命令行及后续处理信息
 *   6. 词法分析阶段：
 *        - 调用 handle_lexer 生成 token 列表
 *        - 打印 token 供调试
 *   7. 如果词法分析失败，释放资源并继续循环
 *   8. 解析阶段：
 *        - 调用 parse_cmdline 生成 AST
 *        - 打印 AST 结构
 *        - 执行 AST（exec_ast）
 *        - 释放 AST 内存
 *   9. 循环结束时释放 lexer、命令行字符串和 t_minishell 结构
 *  10. 退出循环后清理 readline 历史记录
 */

int main(int argc, char *argv[], char **envp)
{
    (void)argc;
    (void)argv;

    char *buf;
    t_minishell *general;
    t_env *env = init_env(envp);
    general = ft_calloc(1, sizeof(t_minishell));
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigint_prompt;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    signal(SIGQUIT, SIG_IGN);

    while (1)
    {
        setup_prompt_signals();
        buf = read_complete_line();
        if (g_signal == SIGINT)
        {
            general->last_exit_status = 130;
            g_signal = 0;
            continue;
        }
        if (!buf)
        {
            printf("exit\n");
            break;
        }

        if (*buf == '\0')
        {
            free(buf);
            continue;
        }
        add_history(buf);

        if (!general)
        {
            perror("calloc");
            free(buf);
            break;
        }
        change_envp(env, &envp);
        general->envp = envp;
        general->raw_line = buf;
        // === Lexer 阶段 ===
        if (handle_lexer(general))
        {
            // printf("Lexer tokens:\n");
            // print_lexer(general->lexer);
        }
        if (!general->lexer)
        {
            fprintf(stderr, "tokenize failed\n");
            free(buf);
            continue;
        }
        //=== expander 阶段 ===
        expander_list(general, general->lexer);
        // === Parser 阶段 ===
        t_lexer *cursor = general->lexer;
        ast *root = parse_cmdline(&cursor, general);
        if (root)
        {
            // printf("=== AST ===\n");
            // print_ast(root, 0);
            int status = exec_ast(root, &env, general);
            // printf("status is  %d\n", status);
            general->last_exit_status = status; // 保存退出码
            free_ast(root);
        }
        else
        {
            // fprintf(stderr, "Parsing failed.\n");
        }
        // === 清理内存 ===
        free_tokens(general->lexer);
        general->lexer = NULL;
        free(buf);
    }
    clear_history();
    free(buf);
    free(env);
    return 0;
}