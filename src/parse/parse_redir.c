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
void process_redir(t_lexer *redir, t_lexer *file, ast *node)
{
    switch (redir->tokentype)
    {
    case TOK_REDIR_IN:
        // 输入重定向 "< filename"
        free(node->redir_in);
        node->redir_in = strdup(file->str);
        break;

    case TOK_REDIR_OUT:
        // 输出重定向 "> filename"
        free(node->redir_out);
        node->redir_out = strdup(file->str);
        break;

    case TOK_APPEND:
        // 追加输出重定向 ">> filename"
        free(node->redir_append);
        node->redir_append = strdup(file->str);
        break;

    case TOK_HEREDOC:
        // heredoc 输入 "<<" delimiter
        free(node->heredoc_delim);
        node->heredoc_delim = strdup(file->str);
        break;

    default:
        // 非法或未知类型（理论上不应发生）
        break;
    }
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
ast *parse_pre_redir(t_lexer **cur, ast *node)
{
    t_lexer *redir; // 保存重定向操作符 token
    t_lexer *file;  // 保存重定向目标文件名 token

    // 取出当前的重定向操作符（例如 '<' 或 '>'）
    redir = consume_token(cur);

    // 下一个 token 必须是文件名（TOK_WORD）
    file = expect_token(TOK_WORD, cur);
    if (!file)
    {
        // 若没有文件名（例如用户输入 "> |"），说明语法错误
        fprintf(stderr, "Syntax error: expected filename after redirection\n");

        // 释放当前命令节点，避免内存泄漏
        free_ast(node);
        return NULL;
    }

    // 将重定向信息填入 AST 节点
    process_redir(redir, file, node);

    return (node);
}
