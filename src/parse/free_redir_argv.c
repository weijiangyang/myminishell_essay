/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_redir_argv.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 16:25:07 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/25 16:25:09 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * free_redir_list
 * ------------------------------------------------------------
 * 目的：
 *   释放重定向链表（t_redir）。每个节点可能包含：
 *     - filename（动态分配的字符串）
 *     - heredoc 的文件描述符（需要在释放前关闭）
 *   函数确保所有资源正确清理，避免 fd 泄漏与内存泄漏。
 *
 * 参数：
 *   @r — 重定向链表的起始节点（可为 NULL）。
 *
 * 返回值：
 *   无（void）
 *
 * 逻辑：
 *   1. 遍历整个 t_redir 链表。
 *   2. 对于每个节点：
 *        - 如果是 HEREDOC 类型，并且 heredoc_fd >= 0，
 *            则关闭文件描述符并设为 -1。
 *        - free(filename)
 *        - free(redir 节点)
 *   3. 前进到 next，直至链表结束。
 *
 * 特性：
 *   - 安全处理 NULL 输入。
 *   - 确保 heredoc 文件描述符不会泄漏。
 *   - 链表完全销毁，不保留悬空指针。
 */
void free_redir_list(t_redir *r)
{
    t_redir *next;

    while (r)
    {
        next = r->next;
        if (r->type == HEREDOC && r->heredoc_fd >= 0)
        {
            close(r->heredoc_fd);
            r->heredoc_fd = -1;
        }
        free(r->filename);
        free(r);
        r = next;
    }
}

/**
 * free_argv_list
 * ------------------------------------------------------------
 * 目的：
 *   释放命令参数链表（t_cmd）。 每个节点通常包含：
 *     - content（动态分配的字符串，表示一个参数 token）
 *     - next 指向下一个参数节点
 *
 * 参数：
 *   @a — 参数链表的起始节点（可为 NULL）。
 *
 * 返回值：
 *   无（void）
 *
 * 逻辑：
 *   1. 遍历整个 t_cmd 链表。
 *   2. 对每个节点：
 *        - free(content)
 *        - free(节点本身)
 *   3. 前进到 next，直至链表结束。
 *
 * 特性：
 *   - 安全处理 NULL 输入。
 *   - 彻底释放整个链表，避免内存泄漏。
 *   - 不会留下悬空指针；局部 next 保存确保遍历安全。
 */
void free_argv_list(t_cmd *a)
{
    t_cmd *next;

    while (a)
    {
        next = a->next;
        free(a->content);
        free(a);
        a = next;
    }
}

/**
 * free_t_cmd_node
 * ------------------------------------------------------------
 * 目的：
 *   释放 t_cmd 链表节点本体（结构体本身），但不释放 content 字符串。
 *   常用于在 argv 构建完成后，仅释放临时链表节点，而保留字符串已经拷贝到 AST 的情况。
 *
 * 参数：
 *   @argv_cmd — t_cmd 链表的起始节点（可为 NULL）。
 *
 * 返回值：
 *   无（void）
 *
 * 逻辑：
 *   1. 遍历整个 t_cmd 链表。
 *   2. 对每个节点：
 *        - 保存 next 指针。
 *        - free(节点本身)。
 *        - 移动到 next。
 *   3. 遍历结束后链表完全释放。
 *
 * 特性：
 *   - 不会释放 content 字符串，避免 double free。
 *   - 安全处理 NULL 输入。
 *   - 链表节点彻底释放，不留悬空指针。
 */
void free_t_cmd_node(t_cmd *argv_cmd)
{
    t_cmd *tmp;
    t_cmd *next;

    tmp = argv_cmd;
    while (tmp)
    {
        next = tmp->next;
        free(tmp);
        tmp = next;
    }
}
