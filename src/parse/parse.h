/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:30:29 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:30:32 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSE_H
#define PARSE_H

#include "../../libft/libft.h"

#define BUFFER_SIZE 42

typedef enum
{
    NODE_CMD,
    NODE_PIPE,
    NODE_AND,
    NODE_OR,
    NODE_SUBSHELL,
    NODE_BACKGROUND,
    NODE_SEQUENCE,
} node_type;

typedef enum e_redir_type
{
    REDIR_INPUT,
    REDIR_OUTPUT,
    REDIR_APPEND,
    HEREDOC,
} t_redir_type;

typedef struct s_redir
{
    struct s_redir *next;
    char *filename;
    int heredoc_fd;
    bool is_expanded;
    t_redir_type type;

} t_redir;
/**
 * @struct s_cmd
 * @brief  单向链表节点，用于在解析阶段暂存命令参数（argv）。
 *
 * 该结构体只在“解析阶段”使用，用于临时保存每一个解析到的
 * 单词（token），最终会被转换为 AST 节点中的 node->argv（char **）。
 *
 * 生命周期与所有权（非常关键）：
 * --------------------------------------------------------------
 * 1) 在解析时，每读到一个 TOK_WORD，就会创建一个 t_cmd 节点：
 *        new->arg = strdup(token->str);
 *
 * 2) 解析完成后，会将整个 t_cmd 链表复制到 node->argv：
 *        node->argv[i] = strdup(tmp->arg);
 *
 * 3) 复制完成后，这个链表中的字符串(new->arg) 及链表节点本身
 *    会由 free_argv_list() 统一释放。
 *
 * 重要：最终的内存所有者是 AST 中的 node->argv（char **）。
 *      t_cmd 链表只是构造阶段的临时容器。
 *
 * 这样设计的好处：
 *  - 避免 node->argv 与临时链表共享同一块字符串（从而避免 double-free）。
 *  - 解析时能方便按顺序收集参数。
 *  - 错误恢复逻辑（free_ast_partial）更简单明确。
 *
 * @field arg   strdup 得到的单个参数字符串（临时所有者）
 * @field next  指向下一个 t_cmd 节点（单向链表）
 */
typedef t_list t_cmd;
typedef struct s_ast
{
    node_type type;
    // 当为node_cmd时
    char **argv;
    t_redir *redir;
    int n_pipes;
    // 当为组合节点时
    struct s_ast *left;
    struct s_ast *right;
    // 当为子shell时
    struct s_ast *sub;
} ast;

void free_ast(ast *node);
void free_tokens(t_lexer *tok);
void free_ast_partial(ast *node);
void free_argv_list(t_cmd *a);
void free_redir_list(t_redir *r);
t_lexer *peek_token(t_lexer **cur);
t_lexer *consume_token(t_lexer **cur);
t_lexer *expect_token(tok_type type, t_lexer **cur);
int is_redir_token(t_lexer *pt);
void print_indent(int depth);
void print_ast(ast *node, int depth);
void print_ast_by_type(ast *node, int depth);
void print_ast_pipe(ast *node, int depth);

void print_ast_cmd(ast *node);
ast *parse_cmdline(t_lexer **cur, t_minishell *minishell);
void print_ast_subshell(ast *node, int depth);
int main(int argc, char *argv[], char **envp);
ast *parse_pipeline(t_lexer **cur, t_minishell *minishell);
ast *parse_subshell(t_lexer **cur, ast *node, t_minishell *minishell);
char *safe_strdup(const char *s);
ast *parse_simple_cmd_redir_list(t_lexer **cur, t_minishell *minishell);
void free_t_cmd_node(t_cmd *argv_cmd);
int heredoc_loop(int write_fd, const char *delimiter);
int handle_heredoc(t_redir *new_redir, t_minishell *minishell);
t_redir *build_redir(t_lexer **cur, ast *node, t_redir *redir, t_minishell *minishell);
char *get_next_line(int fd);
int end_line(char *str);
char *extract_line(char *str);

#endif
