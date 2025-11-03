/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 15:40:59 by yzhang2           #+#    #+#             */
/*   Updated: 2025/10/28 07:25:06 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* include/lexer.h */
#ifndef LEXER_H
# define LEXER_H

# include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include "../libft/libft.h"

// 前置声明：t_general 是程序的总上下文结构，此处不展开定义。

	

// 临时信息包，用来构造 t_lexer 节点：
// clean：去除包裹引号后的文本；
// raw：原始片段（含引号）；
// had_quotes：是否出现过引号；
// quoted_single / quoted_double：是否出现过 ' / "。
typedef struct s_token_info
{
	char					*clean;
	char					*raw;
	int						had_quotes;
	int						quoted_single;
	int						quoted_double;
}							t_token_info;

// 记号类型枚举：
// WORD：普通单词/参数；
// PIPE：|；OR_OR：||；
// GREAT：>；GREAT_GREAT：>>；
// LESS：<；LESS_LESS：<<（heredoc）；
// AMP：&；AND_AND：&&；
// SEMI：;；LPAREN：(；RPAREN：)。
/*typedef enum e_token_type
{
	WORD = 0,
	PIPE,
	OR_OR,
	GREAT,
	GREAT_GREAT,
	LESS,
	LESS_LESS,
	AMP,
	AND_AND,
	SEMI,
	LPAREN,
	RPAREN
}							t_tokens;*/
typedef enum
{
    TOK_WORD,
    TOK_PIPE, // |
    TOK_AND,// &&
    TOK_OR, // ||
    TOK_LPAREN, // (
    TOK_RPAREN, // )
    TOK_REDIR_IN, // <
    TOK_REDIR_OUT,// >
    TOK_APPEND,// >>
    TOK_HEREDOC,// <<
    TOK_END, // EOF
	TOK_AMP, // &
	TOK_SEMI, // ;
    TOK_ERROR
} tok_type;

// 词法链表节点：
// str：节点主文本（通常指去引号后的展示/使用文本）；
// tokentype：本节点的 t_tokens 类型；
// idx：节点编号（创建顺序）；
// had_quotes：该词是否含引号；
// quoted_by：引号类型标记（实现自定：如 0/'/"）；
// raw：原始片段（含引号）；
// prev / next：双向链表指针。
typedef struct s_lexer
{
	char					*str;
	tok_type				tokentype;
	int						idx;
	int						had_quotes;
	int						quoted_by;
	char					*raw;
	struct s_lexer			*prev;
	struct s_lexer			*next;
}							t_lexer;

// remove_quotes拷贝/扫描用索引对：i 为源串读指针，j 为目标串写指针（常用于去引号复制过程）。
typedef struct s_index
{
	size_t					i;
	size_t					j;
}							t_index;

typedef struct s_general
{
	char	*args;
	t_lexer	 *lexer;
}t_general;

t_lexer						*new_node(t_token_info *info, tok_type tokentype);
void						list_add_back(t_lexer **lst, t_lexer *new);
int							add_node(t_token_info *info, tok_type tokentype,
								t_lexer **list);

t_lexer						*clear_one(t_lexer **lst);
void						del_first(t_lexer **lst);
void						del_one(t_lexer **lst, int target);
void						clear_list(t_lexer **lst);

tok_type					is_token(int c);
int							handle_token(char *str, int idx, t_lexer **list);
int							match_quotes(int i, char *str, char quote);

char						*remove_quotes_flag(const char *s, int *had_q,
								int *q_single, int *q_double);

int							handle_word(char *str, int i, t_lexer **list);
int							skip_spaces(char *str, int i);
int							handle_lexer(t_general *general);
int							is_space(char c);
#endif
