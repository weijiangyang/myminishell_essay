#include "../../include/minishell.h"

/*  
** 释放命令节点（NODE_CMD）内部资源的函数  
** 主要负责释放 argv 数组以及所有重定向字符串  
*/
static void free_ast_cmd(ast *node)
{
    int i = 0;

    if (node->argv)
    {
        // 逐个释放命令参数字符串
        while (node->argv[i])
        {
            free(node->argv[i]);
            i++;
        }
        // 最后释放存放指针数组本身
        free(node->argv);
    }

    // 释放命令节点中可能存在的重定向字符串
    free(node->redir_in);       // 输入重定向（<）
    free(node->redir_out);      // 输出重定向（>）
    free(node->redir_append);   // 追加输出重定向（>>）
    free(node->heredoc_delim);  // heredoc 定界符（<<）
}

/*  
** 递归释放整个抽象语法树（AST）的函数  
** 根据节点类型选择不同的释放逻辑  
*/
void free_ast(ast *node)
{
    if (!node)
        return; // 空节点直接返回

    switch (node->type)
    {
        case NODE_CMD:
            // 普通命令节点：释放命令参数和重定向信息
            free_ast_cmd(node);
            break;

        case NODE_PIPE:
        case NODE_AND:
        case NODE_OR:
            // 二元操作节点（管道、逻辑与、逻辑或）：
            // 递归释放左右子树
            free_ast(node->left);
            free_ast(node->right);
            break;

        case NODE_SUBSHELL:
            // 子 shell 节点：递归释放内部子 AST
            free_ast(node->sub);
            break;

        default:
            // 其他未知类型（保险措施）
            break;
    }

    // 最后释放当前节点本身
    free(node);
}

/*  
** 释放词法分析（lexer）阶段生成的 token 链表  
** 每个 t_lexer 节点包含字符串内容和 next 指针  
*/
void free_tokens(t_lexer *tok)
{
    while (tok)
    {
        t_lexer *nx = tok->next; // 保存下一个节点指针，避免 free 后丢失

        if (tok->str)
            free(tok->str);      // 释放当前 token 的字符串内容

        free(tok);               // 释放当前 token 结构体本身
        tok = nx;                // 移动到下一个 token
    }
}
