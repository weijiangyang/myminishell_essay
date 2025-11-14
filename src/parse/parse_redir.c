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

/*
** process_redir
** ----------------
** 处理命令节点 (ast) 的重定向操作。
**
** 参数：
**   - redir : 当前重定向操作符对应的 token（例如 '<', '>', '>>', '<<'）
**   - file  : 重定向目标文件名对应的 token（通常为 TOK_WORD）
**   - node  : 当前命令节点 (AST)，其内部成员将被更新
**
** 功能：
**   根据重定向类型 (redir->tokentype)，将文件名字符串 (file->str)
**   存储到 ast 节点对应的成员中。
**
** 注意：
**   - 若节点中已存在同类型重定向，先释放旧的字符串再保存新值。
**   - strdup() 分配新内存，因此之后在 free_ast_cmd() 中必须释放。
*/
int process_redir(t_lexer *redir, t_lexer *file, ast *node)
{
    char *tmp;

    if (!redir || !file || !node)
        return -1;

    tmp = safe_strdup(file->str);
    if (!tmp)
        return -1;

    switch (redir->tokentype)
    {
    case TOK_REDIR_IN:
        free(node->redir_in);
        node->redir_in = tmp;
        break;

    case TOK_REDIR_OUT:
        free(node->redir_out);
        node->redir_out = tmp;
        break;

    case TOK_APPEND:
        free(node->redir_append);
        node->redir_append = tmp;
        break;

    case TOK_HEREDOC:
        free(node->heredoc_delim);
        node->heredoc_delim = tmp;
        break;

    default:
        /* unknown token, free tmp and treat as error */
        free(tmp);
        return -1;
    }

    return 0;
}

/*
** parse_pre_redir
** ----------------
** 解析单个前置重定向（即出现在命令名前或命令中间的重定向）。
**
** 语法模式（简化）：
**   redirection : ('<' | '>' | '>>' | '<<') WORD
**
** 参数：
**   - cur  : 指向当前 token 游标的指针
**   - node : 当前命令 AST 节点，将被填入重定向信息
**
** 返回：
**   - 成功：返回更新后的 node
**   - 失败：打印错误信息并释放节点，返回 NULL
*/
/*
 * parse_pre_redir - do NOT free(node) inside this function.
 * On error, return NULL; caller must free node.
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
        /* syntax error: leave freeing to caller */
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
