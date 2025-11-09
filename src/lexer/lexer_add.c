/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_add.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/25 22:12:34 by yzhang2           #+#    #+#             */
/*   Updated: 2025/10/28 07:05:48 by yzhang2          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "libft.h"

// 作用：把临时收集到的解析信息填入新建的 `t_lexer` 节点。
// 参数：`new` 新节点；`info` 解析期间的上下文字段。
// 逻辑：字段对字段拷贝（索引、原始子串、去引号文本、标志等），确保节点自包含。（函数体被截断，
// 但从文件命名与 `add_node` 使用看，这里完成初始化）
static void	init_node_info(t_lexer *new, t_token_info *info)
{
	if (info)
	{
		new->str = info->clean;
		new->raw = info->raw;
		new->had_quotes = info->had_quotes;
		new->quoted_by = 0;
		if (info->quoted_single)
			new->quoted_by += 1;
		if (info->quoted_double)
			new->quoted_by += 2;
	}
	else
	{
		new->str = NULL;
		new->raw = NULL;
		new->had_quotes = 0;
		new->quoted_by = 0;
	}
}

// 作用：根据临时解析信息 info 与记号类型 tokentype，
// 分配并构造一个完整的词法节点 t_lexer。
// 参数：
//     * info：词法阶段已收集好的临时信息（比如：原始子串、
// 	去引号后的文本、引号标志、起止索引等）。
//     * tokentype：该节点的 token 类型
// * 实现逻辑简介：
//     1. malloc(sizeof(t_lexer)) 分配新节点，失败返回 NULL。
//     2. 调用 init_node_info(new, info) 把解析期信息拷入节点字段（
// 	确保节点自包含，不依赖外部缓冲）。
//     3. 设置 new->tokentype = tokentype。
//     4. 使用文件静态计数器 static int idx = 0; 给节点分配自增索引：new->idx = idx++;
//     这意味着本进程生命周期内创建的词法节点会获得全局递增的编号；清空链表不会重置该计数器
//     5. 将双向链表指针置空：new->next = NULL; new->prev = NULL;
//     6. 返回新节点指针。
t_lexer	*new_node(t_token_info *info, tok_type tokentype)
{
	t_lexer		*new;
	static int	idx = 0;

	new = malloc(sizeof(t_lexer));
	if (!new)
		return (NULL);
	init_node_info(new, info);
	if (tokentype == TOK_OR)
		new->str = ft_strdup("||");
	if (tokentype == TOK_AND)
		new->str = ft_strdup("&&");
	if (tokentype == TOK_APPEND)
		new->str = ft_strdup(">>");
	if (tokentype == TOK_HEREDOC)
		new->str = ft_strdup("<<");
	if (tokentype == TOK_REDIR_IN)
		new->str = ft_strdup(">");
	if (tokentype == TOK_REDIR_OUT)
		new->str = ft_strdup("<");
	if (tokentype == TOK_AMP)
		new->str = ft_strdup("&");
	if (tokentype == TOK_LPAREN)
		new->str = ft_strdup("(");
	if (tokentype == TOK_RPAREN)
		new->str = ft_strdup(")");
	if (tokentype == TOK_PIPE)
		new->str = ft_strdup("|");
	if (tokentype == TOK_SEMI)
		new->str = ft_strdup(";");
	
	new->tokentype = tokentype;
	new->idx = idx++;
	new->next = NULL;
	new->prev = NULL;
	return (new);
}

// 作用：将 `new` 追加到词法链表尾部。
// 参数：链表头指针地址、待插入节点。
// 逻辑：空表则置头；否则走到尾节点 `next==NULL` 处链接。
void	list_add_back(t_lexer **lst, t_lexer *new)
{
	t_lexer	*tmp;

	if (!lst || !new)
		return ;
	tmp = *lst;
	if (*lst == NULL)
	{
		*lst = new;
		new->prev = NULL;
		return ;
	}
	while (tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = new;
	new->prev = tmp;
	new->next = NULL;
}

// 作用：统一的“构建并追加节点”接口。
// 参数：临时信息、token 类型、链表头地址。
// 逻辑：`new_node(info, tokentype)`（你项目内的构造器）→失败返回 0；
// 成功后 `list_add_back` →返回 1。
int	add_node(t_token_info *info, tok_type tokentype, t_lexer **list)
{
	t_lexer	*node;

	if (!list)
		return (0);
	node = new_node(info, tokentype);
	if (!node)
		return (0);
	list_add_back(list, node);
	return (1);
}
