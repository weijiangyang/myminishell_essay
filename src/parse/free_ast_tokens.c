/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_ast_tokens.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:28:05 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:28:18 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

/**
 * @brief 在解析出错时释放尚未完整连接的 AST 节点。
 *
 * 所有权说明：
 * --------------------------------------------------------------
 * - node->argv 是最终执行要用的 argv（char **），它里面的每个字符串
 *   都是 strdup 得来的，与解析阶段的 t_cmd->arg 没有共享内存。
 *
 * - t_cmd 链表（解析阶段临时 argv）已在其它地方通过 free_argv_list()
 *   释放，因此不会造成 double-free。
 *
 * - 因此这里必须逐个 free node->argv[i]，然后再 free node->argv 本体。
 */
void free_ast_partial(ast *node)
{
    int i;

    i = 0;
    if (!node)
        return;
    if (node->redir)
        free_redir_list(node->redir);
    if (node->argv)
    {
        while (node->argv[i])
        {
            free(node->argv[i]);
            i++;
        }
        free(node->argv);
    }
    free(node);
}
/**
 * free_ast
 * ----------------
 * 目的：
 *   递归释放 AST（抽象语法树）节点及其所有子节点的动态内存。
 *   根据节点类型分别处理：
 *     - NODE_CMD：释放命令节点的 argv 和重定向字段
 *     - NODE_PIPE：递归释放左右子树
 *     - NODE_SUBSHELL：递归释放子 shell 的内部 AST
 *
 * 参数：
 *   - node : 指向要释放的 AST 节点指针（允许为 NULL）
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 注意：
 *   - 最后会 free(node) 自身。
 */

void free_ast(ast *node)
{
    if (!node)
        return;
    if (node->type == NODE_CMD)
    {
        free_ast_partial(node);
        return;
    }
    else if (node->type == NODE_PIPE)
    {
        free_ast(node->left);
        free_ast(node->right);
    }
    else if (node->type == NODE_SUBSHELL)
        free_ast(node->sub);
    free(node);
}

/**
 * free_tokens
 * ----------------
 * 目的：
 *   释放词法分析阶段生成的 token 链表，
 *   包括每个 token 的字符串字段和节点自身。
 *
 * 参数：
 *   - tok : 指向 token 链表头部的指针（允许为 NULL）
 *
 * 返回值：
 *   - 无返回值（void）
 *
 * 行为说明：
 *   - 逐个遍历链表节点
 *   - 先释放 token->str（如果存在）
 *   - 再释放 token 节点本身
 *   - 最终释放整个链表
 */
void free_tokens(t_lexer *tok)
{
    while (tok)
    {
        t_lexer *nx = tok->next;

        if (tok->str)
            free(tok->str);

        free(tok);
        tok = nx;
    }
}


