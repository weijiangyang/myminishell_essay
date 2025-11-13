/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_pipeline.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 17:29:31 by weiyang           #+#    #+#             */
/*   Updated: 2025/11/11 17:29:33 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "../../libft/libft.h"

/*
** parse_pipeline_1
** ----------------
** 辅助函数：用于解析管道操作符 '|' 后的命令。
**
** 语法规则（简化）：
**   pipeline : simple_cmd ('|' simple_cmd)*
**
** 每当遇到一个 '|'，就构建一个新的 AST 节点：
**         PIPE
**        /    \
**   (左命令)  (右命令)
**
** 参数：
**   - cur      : token 游标指针
**   - left     : 当前已解析的左侧命令（或管道）
**   - n_pipes  : 管道计数器，用于统计管道数量
**
** 返回：
**   - 成功：返回更新后的 AST 根节点（最外层 PIPE）
**   - 失败：释放资源并返回 NULL
*/
static ast *parse_pipeline_1(t_lexer **cur, ast **left, int *n_pipes)
{
    // 当下一个 token 是 '|' 时，继续构建管道链
    while (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
    {
        ast *right; // 管道右侧命令
        ast *node;  // 新建的管道 AST 节点

        consume_token(cur); // 消耗掉 '|' token

        // 解析右侧命令：每个管道右边必须是一个 simple_cmd
        right = parse_simple_cmd(cur);
        if (!right)
            // 如果右侧解析失败，释放左子树并返回 NULL
            return (free_ast(*left), NULL);

        // 为新的 PIPE 节点分配内存
        node = ft_calloc(1, sizeof(ast));
        if (!node)
            // 内存分配失败，释放左右子树防止内存泄漏
            return (free_ast(*left), free_ast(right), NULL);

        // 设置当前节点类型为 PIPE，并连接左右子树
        node->type = NODE_PIPE;
        node->left = *left;   // 左边是前一个命令或管道
        node->right = right;  // 右边是新解析到的命令

        // 管道计数 +1
        (*n_pipes)++;

        // 将当前节点更新为新的根节点（嵌套管道向上生长）
        *left = node;
    }

    // 返回最终的管道根节点
    return (*left);
}

/*
** parse_pipeline
** ----------------
** 解析一条可能包含多个管道的命令。
**
** 举例：
**   输入: ls -la | grep main | wc -l
**
** 生成的 AST 结构：
**          PIPE
**         /    \
**     PIPE      CMD("wc -l")
**    /    \
** CMD("ls -la") CMD("grep main")
**
** 参数：
**   - cur : token 游标指针
**
** 返回：
**   - 成功：返回完整的管道 AST 根节点
**   - 失败：返回 NULL
*/
ast *parse_pipeline(t_lexer **cur)
{
    ast *left;    // 当前管道最左侧命令节点
    int n_pipes;  // 管道数量统计

    // 首先解析第一个命令（管道左端）
    left = parse_simple_cmd(cur);
    if (!left)
        return NULL;

    n_pipes = 0; // 初始化管道计数器

    // 递归解析后续的 '|' 连接部分
    parse_pipeline_1(cur, &left, &n_pipes);

    // 将管道数量记录到最外层节点中（便于调试或执行阶段使用）
    left->n_pipes = n_pipes;

    // 返回完整的管道结构
    return left;
}
