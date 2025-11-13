#include "../../include/minishell.h"

static void	toggle_quote_state(const char c, enum qstate *q)
{
    if (c == '\'' && *q != Q_DQ)
    {
        if (*q == Q_SQ)
            *q = Q_NONE;
        else
            *q = Q_SQ;
    }
    else if (c == '\"' && *q != Q_SQ)
    {
        if (*q == Q_DQ)
            *q = Q_NONE;
        else
            *q = Q_DQ;
    }
}


// 做什么：把单字符 c 追加到 *out（用 str_join_free）。
// 输出：固定 1（表示“我消费了 1 个字符”）。
// 谁调：expand_all 遇到普通字符时。
static int	append_char(const char c, char **out)
{
	char	buf[2];

	buf[0] = c;
	buf[1] = '\0';
	*out = str_join_free(*out, buf);
	return (1);
}


// 做什么：整串展开（保留引号字符）
// 初始化 out=""、q=Q_NONE；
// 遍历 str[i]：
// 先 toggle_quote_state(str[i])；
// 若 str[i] == '$' → i += scan_expand_one(&data, str, i, q)；
// 否则 → i += append_char(str[i], &out)；
// 返回 out（堆串）。
// 输入：minishell（为了 $?/env）、str 原始片段（最好是 raw）。
// 输出：新堆串（只做 $ 展开，不去引号）。
// 谁调：expand_token、expander_str、测试。
char	*expand_all(t_minishell *minishell, const char *str)
{
	int			i;
	char		*out;
	enum qstate	q;
	t_exp_data	data;

	if (!str)
		return (NULL);
	i = 0;
	q = Q_NONE;
	out = ft_strdup("");
	if (!out)
		return (NULL);
	data.minishell = minishell;
	data.out = &out;
	while (str[i])
	{
		toggle_quote_state(str[i], &q);
		if (str[i] == '$')
			i += scan_expand_one(&data, str, i, q);
		else
			i += append_char(str[i], &out);
	}
	return (out);
}