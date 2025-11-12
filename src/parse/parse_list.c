#include "../../include/minishell.h"

ast *parse_list(t_lexer **cur)
{
    ast *left = parse_and_or(cur);
    if (!left) return NULL;

    while (peek_token(cur) &&
           (peek_token(cur)->tokentype == TOK_SEMI ||
            peek_token(cur)->tokentype == TOK_AMP))
    {
        t_lexer *op = consume_token(cur);
        ast *node = calloc(1, sizeof(ast));
        node->type = (op->tokentype == TOK_AMP) ? NODE_BACKGROUND : NODE_SEQUENCE;
        node->left = left;
        node->right = parse_list(cur);
        left = node;
    }

    return left;
}