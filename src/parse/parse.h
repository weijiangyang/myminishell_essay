#ifndef PARSE_H
#define PARSE_H


typedef enum
{
    NODE_CMD,
    NODE_PIPE,
    NODE_AND,
    NODE_OR,
    NODE_SUBSHELL
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
} ast;

// 全局或上下文中的token游标
static t_lexer *g_cur;
ast *parse_simple_cmd(t_lexer **cur);
ast *parse_pipeline(t_lexer **cur);
ast *parse_and_or(t_lexer **cur);
ast *parse_cmdline(t_lexer **cur);

#endif