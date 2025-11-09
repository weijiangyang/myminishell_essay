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

#ifndef LEXER_H
#define LEXER_H

// 临时信息包，用来构造 t_lexer 节点：
// clean：去除包裹引号后的文本；
// raw：原始片段（含引号）；
// had_quotes：是否出现过引号；
// quoted_single / quoted_double：是否出现过 ' / "。
typedef struct s_token_info
{
	char *clean;
	char *raw;
	int had_quotes;
	int quoted_single;
	int quoted_double;
} t_token_info;

typedef struct s_index
{
	size_t i;
	size_t j;
} t_index;

typedef enum
{
	TOK_WORD,
	TOK_PIPE,	   // |
	TOK_AND,	   // &&
	TOK_OR,		   // ||
	TOK_LPAREN,	   // (
	TOK_RPAREN,	   // )
	TOK_REDIR_IN,  // <
	TOK_REDIR_OUT, // >
	TOK_APPEND,	   // >>
	TOK_HEREDOC,   // <<
	TOK_END,	   // EOF
	TOK_AMP,	   // &
	TOK_SEMI,	   // ;
	TOK_ERROR
} tok_type;

typedef struct s_lexer
{
	char *str;
	tok_type tokentype;
	int idx;
	int had_quotes;
	int quoted_by;
	char *raw;
	struct s_lexer *prev;
	struct s_lexer *next;
} t_lexer;

t_lexer *new_node(t_token_info *info, tok_type tokentype);
void list_add_back(t_lexer **lst, t_lexer *new);
int add_node(t_token_info *info, tok_type tokentype,
			 t_lexer **list);

t_lexer *clear_one(t_lexer **lst);
void del_first(t_lexer **lst);
void del_one(t_lexer **lst, int target);
void clear_list(t_lexer **lst);

tok_type is_token(int c);
int handle_token(char *str, int idx, t_lexer **list);
int match_quotes(int i, char *str, char quote);

char *remove_quotes_flag(const char *s, int *had_q,
						 int *q_single, int *q_double);

int handle_word(char *str, int i, t_lexer **list);
int skip_spaces(char *str, int i);
int handle_lexer(t_minishell *general);
int is_space(char c);

void print_lexer(t_lexer *lexer);

#endif
