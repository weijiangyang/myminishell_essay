#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024

#endif

// 内置命令结构体
typedef struct s_builtin
{
    char *builtin_name;
    int (*foo)(char **);
} t_builtin;

typedef enum e_token_type
{
    WORD = 0,
    PIPE,        //'|' 
    OR_OR,       // '||' 
    GREAT,       //'>' 
    GREAT_GREAT, //'>>'
    LESS,        //'<' 
    LESS_LESS,   // '<<'
    AMP,         //'&' 
    AND_AND,     //'&&'
    SEMI,        //';'
    LPAREN,      //'('
    RPAREN       // ')'
}   t_tokens;

typedef struct s_token_info
{
    char clean;        // 去引号后的字符串，parser 用于 filename/delimiter
    char raw;          // 原始含引号片段（若存在），parser 用于判定 delimiter 是否被引号包裹
    int had_quotes;    // 1 如果包含任意引号；parser 用于决定是否做某些扩展
    int quoted_single; // 1 如果包含单引号（parser 可据此判断单引号语义）
    int quoted_double; // 1 如果包含双引号（parser 可据此判断双引号语义）
} t_token_info;

/*t_lexer
 *
 * 词法链表节点：每个节点对应一个 token。lexer 生成链表，parser 会遍历
 * 该链表并根据 tokentype/str/raw/had_quotes 等信息构建 AST/命令结构。
 * 下面只在 parser 会用到的字段行添加了短注释，便于实现 parse 阶段时查阅。
 */
typedef struct s_lexer
{
    char            *str;        /* 去引号后的字符串，parser 用于 open/exec/比较 delimiter */
    t_tokens        tokentype;   /* token 类型，parser 根据它决定语法角色（word/pipe/redirect） */
    int                idx;         /* 节点序号，parser/delete 操作时用于定位/调试 */
    int                had_quotes;  /* 是否含引号（parser 用于决定扩展/匹配行为） */
    int                quoted_by;   /* 0-none,1-single,2-double,3-both；parser 可据此进一步判断 */
    char            *raw;        /* 原始含引号文本（仅当需要时存在），parser 在 heredoc/delimiter 判定时使用 */
    struct s_lexer    *prev;
    struct s_lexer    *next;       /* 链表遍历：parser 从这里顺序读取 token */
}                t_lexer;

typedef struct s_general
{
    t_lexer *lexer; // tête du lexer
    int some_value; // exemple de membre
    char **env;     // exemple : environnement
    // ajoute ici tous les autres champs nécessaires
} t_general;

void getcwd_wrapped(char *buf, size_t size);
void *malloc_wrapper(size_t size);
void *realloc_wrapper(void *ptr, size_t size);
int cell_exit(char **args);
int cell_env(char **args);
pid_t fork_wrapper();
void execvp_wrapper(const char *file, char *const av[]);
int wait_wrapper(int *status);
int cell_echo(char **args);

#endif