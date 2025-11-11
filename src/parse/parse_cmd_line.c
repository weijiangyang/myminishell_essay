/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yang <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 22:19:34 by yang              #+#    #+#             */
/*   Updated: 2025/10/31 22:19:38 by yang             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
/*  
** parse_cmdline
** ----------------
** 这是整个命令解析过程的“最高层入口函数”，负责解析完整的一行命令。
**
** 语法规则（简化版）：
**   cmdline : and_or TOK_END
**
** 它会调用 parse_and_or() 来构建完整的语法树（AST），
** 然后检查是否正确到达输入结尾（防止多余 token）。
**
** 参数：
**   - cur : 指向当前 token 指针的指针（即词法分析器的游标）
**
** 返回值：
**   - 成功：返回完整语法树 (AST*)
**   - 失败：返回 NULL
*/
ast *parse_cmdline(t_lexer **cur)
{
    ast *root;     // 整个命令行的 AST 根节点
    t_lexer *pt;   // 用于检查当前 token

    // 调用下一级解析函数，解析由 AND/OR/Pipeline/Command 组成的语句结构
    root = parse_and_or(cur);
    if (!root)
        // 若解析失败（例如语法错误或内存分配失败），直接返回 NULL
        return NULL;

    // 查看当前游标指向的下一个 token（但不移动指针）
    pt = peek_token(cur);

    // 如果还有 token 且不是表示行结束的 TOK_END（即多余内容），则报语法错误
    if (pt && pt->tokentype != TOK_END)
    {
        fprintf(stderr, "Syntax error: unexpected token at end (type %d)\n", pt->tokentype);
        // 清理 AST，防止内存泄漏
        free_ast(root);
        return NULL;
    }

    // 若成功解析完整命令且无多余 token，则返回 AST 根节点
    return root;
}

/*

t_ast *parse_command_line(t_lexer **cur)
{
    t_ast *root = NULL;
    t_lexer *tok;

    while ((tok = peek_token(cur)) && tok->tokentype != TOK_END)
    {
        t_ast *cmd = parse_normal_cmd_redir(cur, new_ast_node(), tok);
        if (!cmd)
            return (free_ast(root), NULL);

        // 链接到 root
        append_ast(&root, cmd);

        // 看是否是后台执行
        tok = peek_token(cur);
        if (tok && tok->tokentype == TOK_AMP)
        {
            consume_token(cur);
            cmd->is_background = 1;
            continue; // 继续解析下一个命令（echo hello）
        }

        // 其它分隔符 (如 ;)
        if (tok && tok->tokentype == TOK_SEMI)
            consume_token(cur);
    }

    return root;
}

这样，ls -la & echo hello 会被解析成两棵命令节点：
| 命令           | is_background |
| ------------ | ------------- |
| `ls -la`     | 1             |
| `echo hello` | 0             |

⚙️ 3. 在 parse_normal_cmd_redir 里别吃掉下一个命令的 token

当前函数可能消费了过多 token（包括 & 后面的部分）。
确保在遇到 & 时只：

设置 node->is_background = 1

返回到上层 parser，由它继续解析下一个命令。

即不要在 parse_normal_cmd_redir 里试图递归解析 echo hello。

✅ 4. 总结修复方向
目标	说明
Lexer	& → 生成 TOK_AMP ✅（你已经做到了）
Parser (parse_normal_cmd_redir)	只负责当前命令，遇到 & 设置 flag 后返回
Parser 主循环 (parse_command_line)	继续解析下一个命令（分隔符 & 或 ;）
执行阶段	如果节点 is_background=1，fork 后不 wait
*/









