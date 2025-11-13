/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 18:31:27 by yzhang2           #+#    #+#             */
/*   Updated: 2025/11/12 19:11:13 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_H
#define EXPANDER_H

#include "../../libft/libft.h"

typedef struct s_minishell t_minishell;
typedef struct s_lexer t_lexer;

/* 引号状态机（字符级扫描时使用）
 * 作用：expand_all/scan_expand_one 在**逐字符**扫描时，随时知道
 * 当前字符处于什么引号环境，从而决定 $ 是否展开。
 * - Q_NONE：不在引号里；
 * - Q_SQ  ：在单引号 '...' 内 → 禁止 $ 展开；
 * - Q_DQ  ：在双引号 "..." 内 → 允许 $ 展开（仅做变量替换，不干预分词/通配）。
 */
enum qstate
{
	Q_NONE = 0,
	Q_SQ = 1,
	Q_DQ = 2
};

/* 扩展时的临时“小包”（传参用）
 * 作用：把全局上下文和“输出字符串指针”打包传给字符级函数。
 * 字段说明：
 * - minishell：指向全局上下文（读 envp、last_exit_status 等）；
 * - out      ：指向“输出字符串”的指针地址。字符级函数会不断拼接内容，
 *              需要能修改调用者持有的指针（所以是 char **）。
 */
typedef struct s_exp_data
{
	t_minishell *minishell;
	char **out;
} t_exp_data;

int expander_list(t_minishell *minishell,
				  t_lexer *head);
char *expander_str(t_minishell *minishell, char *str);

int scan_expand_one(t_exp_data *data, const char *s,
					int j, enum qstate q);
int expand_token(t_minishell *msh, t_lexer *node,
				 int export_mode);
char *expand_all(t_minishell *minishell,
				 const char *str);

int is_name_start(int c);
int is_name_char(int c);
int var_len(const char *s);
char *env_value_dup(t_minishell *minishell,
					const char *name, int len);

char *str_join_free(char *a, const char *b);
size_t equal_sign(const char *entry);

#endif
