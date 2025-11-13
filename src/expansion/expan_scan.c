#include "../../include/minishell.h"

// 做什么：res = a + b，并 free(a)；若 a==NULL 则相当于 strdup(b)。
// 谁调：append_char、scan_expand_one、handle_*_exp。
char	*str_join_free(char *a, const char *b)
{
	char	*res;

	if (!a && !b)
		return (NULL);
	if (!a)
		return (ft_strdup(b));
	if (!b)
		return (a);
	res = ft_strjoin(a, b);
	free(a);
	return (res);
}

// 做什么：处理特殊 $：
// $? → 追加 itoa(last_exit_status)，返回消费 2；
// $<digit> → 空展开（什么也不追加），返回消费 2；
// 其他情况返回 0（表示“我没处理，你去走正常变量路径”）。
// 谁调：scan_expand_one 的第一步。
static int	handle_special_exp(t_exp_data *data, const char *s, int j)
{
	char	*tmp;

	if (s[j + 1] == '?')
	{
		tmp = ft_itoa(data->minishell->last_exit_status);
		if (!tmp)
			return (2);
		*data->out = str_join_free(*data->out, tmp);
		free(tmp);
		return (2);
	}
	if (ft_isdigit((unsigned char)s[j + 1]))
		return (2);
	return (0);
}

// 做什么：处理 $VAR：
// 计算变量名长度 len = var_len(&s[j+1])；
// 若 len>0：取值 tmp = env_value_dup(...) 并追加；返回消费 1+len；
// 否则：把 $ 当普通字符追加，返回消费 1。
// 谁调：scan_expand_one 的第二步（当特殊路径没命中时）。
static int	handle_var_exp(t_exp_data *data, const char *s, int j)
{
	int		len;
	char	*tmp;

	len = var_len(&s[j + 1]);
	if (len > 0)
	{
		tmp = env_value_dup(data->minishell, &s[j + 1], len);
		*data->out = str_join_free(*data->out, tmp);
		free(tmp);
		return (1 + len);
	}
	*data->out = str_join_free(*data->out, "$");
	return (1);
}

// 做什么：展开一次从 s[j] 开始的 $...：
// 若在单引号 Q_SQ：不展开，直接追加 $，返回 1；
// 否则先试特殊规则 → 若命中返回消费数；
// 否则走变量规则 → 返回消费数。
// 输入：上下文 data、源串 s、位置 j、引号状态 q。
// 输出：消费的字符数（供 expand_all 前进用）。
// 谁调：expand_all。
int	scan_expand_one(t_exp_data *data, const char *s, int j, enum qstate q)
{
	int	res;

	if (q == Q_SQ)
	{
		*data->out = str_join_free(*data->out, "$");
		return (1);
	}
	res = handle_special_exp(data, s, j);
	if (res > 0)
		return (res);
	res = handle_var_exp(data, s, j);
	return (res);
}