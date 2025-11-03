#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
    TOK_WORD,
    TOK_PIPE,
    TOK_AND,
    TOK_OR,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_REDIR_IN,
    TOK_REDIR_OUT,
    TOK_APPEND,
    TOK_HEREDOC,
    TOK_END,
    TOK_ERROR
} tok_type;

typedef struct s_token
{
    tok_type type;
    char *text;
    struct s_token *next;
} token;

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
static token *g_cur;
static ast *parse_simple_cmd(token **cur);
static ast *parse_pipeline(token **cur);
static ast *parse_and_or(token **cur);
static ast *parse_cmdline(token **cur);