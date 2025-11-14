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

typedef struct s_ast
{
    node_type type;
    // 当为node_cmd时
    char **argv;
    char *redir_in;
    char *redir_out;
    char *redir_append;
    char *heredoc_delim;
    int n_pipes;
    // 当为组合节点时
    struct s_ast *left;
    struct s_ast *right;
    // 当为子shell时
    struct s_ast *sub;
    int is_background; // 1 表示后台执行
} ast;

ast *parse_simple_cmd(t_lexer **cur);
ast *parse_pipeline(t_lexer **cur);
ast *parse_and_or(t_lexer **cur);
ast *parse_cmdline(t_lexer **cur);
void free_ast(ast *node);
void free_tokens(t_lexer *tok);
t_lexer *peek_token(t_lexer **cur);
t_lexer *consume_token(t_lexer **cur);
t_lexer *expect_token(tok_type type, t_lexer **cur);
int is_redir_token(t_lexer *pt);
void print_indent(int depth);
void print_ast(ast *node, int depth);
void print_ast_by_type(ast *node, int depth);
void print_ast_pipe(ast *node, int depth);
void print_ast_and(ast *node, int depth);
void print_ast_or(ast *node, int depth);
void print_ast_subshell(ast *node, int depth);
void print_ast_cmd(ast *node);
ast *parse_cmdline(t_lexer **cur);
int main(int argc, char *argv[]);
ast *parse_and_or(t_lexer **cur);
ast *parse_pipeline(t_lexer **cur);
int process_redir(t_lexer *redir, t_lexer *file, ast *node);
ast *parse_pre_redir(t_lexer **cur, ast *node);
ast *parse_subshell(t_lexer **cur, ast *node);
ast *parse_list(t_lexer **cur);
char *safe_strdup(const char *s);

#endif
