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

/**
 * read_complete_line
 * ----------------
 * 目的：
 *   从用户读取一行命令，处理多行输入情况（未闭合引号时继续提示）。
 *
 * 返回值：
 *   - 成功：返回完整的用户输入字符串
 *   - 用户中断或 EOF：返回 NULL
 *
 * 行为说明：
 *   1. 使用 readline 提示 "minishell$> " 获取第一行输入
 *   2. 检查输入中是否存在未闭合的引号：
 *        - 如果存在，提示 "> " 继续读取下一行
 *        - 将上一行与新行拼接，并释放旧行
 *   3. 循环直到所有引号闭合或用户输入 EOF/中断
 *   4. 返回完整命令行字符串
 */
static char *read_complete_line(void)
{
    char *line = NULL;
    char *next = NULL;

    line = readline("minishell$> ");
    while (has_unclosed_quotes(line))
    {
        next = readline("> ");
        if (!next)
            break;
        line = ft_strjoin_free(line, next, 1, 1);
    }
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

    while (1)
    {
        buf = read_complete_line();
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
        general = ft_calloc(1, sizeof(t_minishell));
        if (!general)
        {
            perror("calloc");
            free(buf);
            break;
        }
        general->envp = envp;
        general->raw_line = buf;
        // === Lexer 阶段 ===
        if (handle_lexer(general))
        {
            printf("Lexer tokens:\n");
            print_lexer(general->lexer);
        }
        if (!general->lexer)
        {
            fprintf(stderr, "tokenize failed\n");
            free(buf);
            free(general);
            continue;
        }
        //=== expander 阶段 ===
        expander_list(general, general->lexer);
        // === Parser 阶段 ===
        t_lexer *cursor = general->lexer;
        ast *root = parse_cmdline(&cursor, general);
        if (root)
        {
            printf("=== AST ===\n");
            print_ast(root, 0);
            exec_ast(root);
            free_ast(root);
        }
        else
        {
            fprintf(stderr, "Parsing failed.\n");
        }
        // === 清理内存 ===
        free_tokens(general->lexer);
        free(buf);
        free(general);
    }
    clear_history();
    return 0;
}