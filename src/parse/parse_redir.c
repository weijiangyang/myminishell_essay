/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_redir.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:29:46 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:29:47 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

/**
 * ft_lstadd_back_1
 * ----------------
 * 目的：
 *   将一个 t_redir 类型的新节点追加到重定向链表的末尾。
 *
 * 参数：
 *   - lst : 指向 t_redir 链表头指针的指针
 *   - new : 指向要追加的新节点
 *
 * 返回值：
 *   - void : 无返回值，链表头可能会被修改（如果原链表为空）
 *
 * 行为说明：
 *   1. 检查 lst 和 new 是否为 NULL，若为 NULL 则直接返回
 *   2. 如果链表为空（*lst 为 NULL），直接将 *lst 指向 new
 *   3. 如果链表不为空：
 *      a. 遍历链表找到最后一个节点
 *      b. 将最后一个节点的 next 指针指向 new，从而追加节点
 */
static void	ft_lstadd_back_1(t_redir **lst, t_redir *new)
{
	t_redir	*last;

	if (!lst || !new)
		return ;
	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	last = *lst;
	while (last->next)
		last = last->next;
	last->next = new;
}

/**
 * ft_lstnew_1
 * ----------------
 * 目的：
 *   创建一个新的 t_redir 链表节点，将 content 作为节点的数据，并初始化 next 为 NULL。
 *
 * 参数：
 *   - content : 要存储在节点中的内容，一般是指向文件名的指针
 *
 * 返回值：
 *   - t_redir* : 新创建的节点指针
 *   - NULL     : 内存分配失败
 *
 * 行为说明：
 *   1. 使用 malloc 为 t_redir 结构分配内存
 *   2. 检查分配是否成功，如果失败返回 NULL
 *   3. 将 content 赋值给新节点的 filename 字段
 *   4. 将 next 初始化为 NULL
 *   5. 返回新节点指针
 */
static t_redir	*ft_lstnew_1(void *content)
{
	t_redir	*new_node;

	new_node = (t_redir *)malloc(sizeof(t_redir));
	if (!new_node)
		return (NULL);
	new_node->filename = content;
	new_node->next = NULL;
	return (new_node);
}

/**
 * process_redir_1
 * ----------------
 * 目的：
 *   将单个重定向（输入/输出/追加/HEREDOC）加入到 AST 命令节点对应的重定向链表或字段中。
 *
 * 参数：
 *   - type : 重定向类型（TOK_REDIR_IN, TOK_REDIR_OUT, TOK_APPEND, TOK_HEREDOC）
 *   - node : 指向 AST 命令节点
 *   - tmp  : 重定向目标文件名字符串（或 HEREDOC 分隔符），由调用者分配
 *
 * 返回值：
 *   - 0  : 成功处理重定向
 *   - -1 : 参数非法或重定向类型未知，释放 tmp 后返回
 *
 * 行为说明：
 *   1. 调用 ft_lstnew_1 创建一个 t_redir 节点，将 tmp 作为 filename
 *   2. 根据重定向类型，将新节点追加到 AST 对应链表：
 *        - TOK_REDIR_IN   -> node->redir_in
 *        - TOK_REDIR_OUT  -> node->redir_out
 *        - TOK_APPEND     -> node->redir_append
 *   3. 如果是 HEREDOC（TOK_HEREDOC）：
 *        - 释放 node->heredoc_delim 原来的值
 *        - 将 tmp 直接赋给 node->heredoc_delim
 *   4. 如果重定向类型不支持：
 *        - 释放 tmp
 *        - 返回 -1
 *   5. 成功处理完成后返回 0
 */
int process_redir_1(tok_type type, ast *node, char *tmp)
{
    t_redir *tmp_redir;

    tmp_redir = ft_lstnew_1(tmp);
    if (type == TOK_REDIR_IN)
        ft_lstadd_back_1(&node->redir_in, tmp_redir);
    else if (type == TOK_REDIR_OUT)
        ft_lstadd_back_1(&node->redir_out, tmp_redir);
    else if (type == TOK_APPEND)
        ft_lstadd_back_1(&node->redir_append, tmp_redir);
    else if (type == TOK_HEREDOC)
    {
        free(node->heredoc_delim);
        node->heredoc_delim->delim = tmp;
    }
    else
        return (free(tmp), -1);
    return (0);
}

/**
 * process_redir
 * ----------------
 * 目的：
 *   处理单个重定向操作，将 token 指向的文件名复制到 AST 命令节点
 *   的对应重定向字段。
 *
 * 参数：
 *   - redir : 指向重定向操作符 token（如 '<', '>', '>>', '<<'）
 *   - file  : 指向重定向目标文件名 token
 *   - node  : 指向 AST 命令节点
 *
 * 返回值：
 *   - 0  : 成功处理重定向
 *   - -1 : 参数非法或内存分配失败
 *
 * 行为说明：
 *   1. 检查 redir、file 和 node 是否为 NULL
 *   2. 使用 safe_strdup 复制文件名字符串，确保分配安全
 *   3. 调用 process_redir_1 将 tmp 设置到 AST 节点对应字段
 *      并释放旧值（若存在）
 *   4. 返回 process_redir_1 的执行结果
 */
int process_redir(t_lexer *redir, t_lexer *file, ast *node)
{
    char *tmp;

    if (!redir || !file || !node)
        return -1;
    tmp = safe_strdup(file->str);
    if (!tmp)
        return -1;
    return (process_redir_1(redir->tokentype, node, tmp));
}

/**
 * parse_pre_redir
 * ----------------
 * 目的：
 *   解析单个前置重定向（命令前或命令中间出现的 '<', '>', '>>', '<<'）
 *   并将目标文件或 heredoc 保存到 AST 命令节点中。
 *
 * 参数：
 *   - cur  : 指向当前 token 游标的指针
 *   - node : 指向 AST 命令节点
 *
 * 返回值：
 *   - 成功：返回更新后的 node
 *   - 失败：返回 NULL（语法错误或内部错误），由调用者负责释放 node
 *
 * 行为说明：
 *   1. 从 cur 获取重定向操作符 token
 *      - 若缺失 token，打印内部错误并返回 NULL
 *   2. 获取重定向目标 token（必须为 TOK_WORD）
 *      - 若缺失，打印语法错误并返回 NULL
 *   3. 调用 process_redir 将目标字符串设置到 AST 节点对应字段
 *      - 若失败，打印内部错误并返回 NULL
 *   4. 返回更新后的 node
 */
ast *parse_pre_redir(t_lexer **cur, ast *node)
{
    t_lexer *redir;
    t_lexer *file;

    if (!cur || !*cur || !node)
        return NULL;
    redir = consume_token(cur);
    if (!redir)
    {
        fprintf(stderr, "internal parser error: missing redir token\n");
        return NULL;
    }
    file = expect_token(TOK_WORD, cur);
    if (!file)
    {
        fprintf(stderr, "Syntax error: expected filename after redirection\n");
        return NULL;
    }
    if (process_redir(redir, file, node) < 0)
    {
        fprintf(stderr, "internal error: failed to process redirection\n");
        return NULL;
    }
    return node;
}
