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

static void free_ast(ast *node);
static void free_tokens(t_lexer *tok);

/* ---------- token cursor API (cursor is token **cur) ---------- */

/* peek at current token (does not advance) */
static t_lexer *peek_token(t_lexer **cur)
{
    if (!cur)
        return NULL;
    return *cur;
}

/* consume current token and advance cursor.
   returns the consumed token (old current) or NULL */
static t_lexer *consume_token(t_lexer **cur)
{
    if (!cur || !*cur)
        return NULL;
    t_lexer *old = *cur;
    *cur = (*cur)->next;
    return old;
}

/* ensure current token has the given type; consume it and return the consumed token.
   returns NULL on mismatch */
static t_lexer *expect_token(tok_type type, t_lexer **cur)
{
    if (!cur || !*cur || (*cur)->tokentype != type)
    {
        fprintf(stderr, "Syntax error : expected token type %d\n", type);
        return NULL;
    }
    return consume_token(cur);
}

static int is_redir_token(t_lexer *pt)
{
    if (pt->tokentype == TOK_REDIR_IN || pt->tokentype == TOK_REDIR_OUT || pt->tokentype == TOK_APPEND || pt->tokentype == TOK_HEREDOC)
        return (1);
    else
        return (0);
}

ast *parse_subshell(t_lexer **cur, ast *node)
{
    consume_token(cur);
    node->type = NODE_SUBSHELL;
    node->sub = parse_and_or(cur);
    if (!expect_token(TOK_RPAREN, cur))
    {
        fprintf(stderr, "Syntax error: expected ')'\n");
        free_ast(node);
        return NULL;
    }
    return node;
}

void process_redir(t_lexer *redir, t_lexer *file, ast *node)
{
    switch (redir->tokentype)
    {
    case TOK_REDIR_IN:
        free(node->redir_in);
        node->redir_in = strdup(file->str);
        break;
    case TOK_REDIR_OUT:
        free(node->redir_out);
        node->redir_out = strdup(file->str);
        break;
    case TOK_APPEND:
        free(node->redir_append);
        node->redir_append = strdup(file->str);
        break;
    case TOK_HEREDOC:
        free(node->heredoc_delim);
        node->heredoc_delim = strdup(file->str);
        break;
    default:
        break;
    }
}
ast *parse_pre_redir(t_lexer **cur, ast *node)
{
    t_lexer *redir;
    t_lexer *file;

    redir = consume_token(cur);
    file = expect_token(TOK_WORD, cur);
    if (!file)
    {
        fprintf(stderr, "Syntax error: expected filename after redirection\n");
        free_ast(node);
        return NULL;
    }
    process_redir(redir, file, node);
    return (node);
}

ast *parse_cmd_argv(t_lexer **cur, ast *node, size_t *argv_cap, size_t *argc)
{
    t_lexer *t;
    char **tmp;

    t = consume_token(cur);
    if (*argc + 1 >= *argv_cap)
    {
        *argv_cap *= 2;
        tmp = realloc(node->argv, *argv_cap * sizeof(char *));
        if (!tmp)
        {
            fprintf(stderr, "realloc failed\n");
            free_ast(node);
            return NULL;
        }
        node->argv = tmp;
    }
    node->argv[*argc] = strdup(t->str);
    (*argc)++;
    return (node);
}

void *parse_normal_cmd_redir(t_lexer **cur, ast *node, t_lexer *pt)
{
    size_t argv_cap;
    size_t argc;
    t_lexer *t;

    argv_cap = 8;
    argc = 0;
    node->argv = calloc(argv_cap, sizeof(char *));
    if (!node->argv)
        return (free(node), NULL);
    if (!pt || pt->tokentype != TOK_WORD)
        return (fprintf(stderr, "Syntax error: expected command name\n"), free_ast(node), NULL);
    t = consume_token(cur);
    node->argv[argc] = strdup(t->str);
    argc++;
    while ((pt = peek_token(cur)) && (pt->tokentype == TOK_WORD || is_redir_token(pt)))     
    {
        if (pt->tokentype == TOK_WORD)
            parse_cmd_argv(cur, node, &argv_cap, &argc);
        else
            parse_pre_redir(cur, node);
    }
    node->argv[argc] = NULL;
    return (node);
}

ast *parse_normal_cmd(t_lexer **cur, ast *node)
{
    size_t argv_cap;
    size_t argc;
    t_lexer *pt;

    node->type = NODE_CMD;
    argv_cap = 8;
    argc = 0;
    node->argv = calloc(argv_cap, sizeof(char *));
    if (!node->argv)
    {
        free(node);
        return NULL;
    }
    while ((pt = peek_token(cur)) && is_redir_token(pt))
        parse_pre_redir(cur, node);
    parse_normal_cmd_redir(cur, node, pt);
    return (node);
}

ast *parse_simple_cmd(t_lexer **cur)
{
    ast *node;
    t_lexer *pt;

    pt = peek_token(cur);
    node = calloc(1, sizeof(ast));
    if (!node)
        return (NULL);
    if (pt && pt->tokentype == TOK_LPAREN)
        return (parse_subshell(cur, node));

    /* normal command */
    return (parse_normal_cmd(cur, node));
}

ast *parse_pipeline(t_lexer **cur)
{
    ast *left = parse_simple_cmd(cur);
    int n_pipes = 0;
    if (!left)
        return NULL;
    while (peek_token(cur) && peek_token(cur)->tokentype == TOK_PIPE)
    {
        consume_token(cur);
        ast *right = parse_simple_cmd(cur);
        if (!right)
        {
            free_ast(left);
            return NULL;
        }
        ast *node = calloc(1, sizeof(ast));
        if (!node)
        {
            free_ast(left);
            free_ast(right);
            return NULL;
        }
        node->type = NODE_PIPE;
        node->left = left;
        node->right = right;
        n_pipes++;
        left = node;
    }
    left->n_pipes = n_pipes;
    return left;
}

ast *parse_and_or(t_lexer **cur)
{
    ast *left = parse_pipeline(cur);
    if (!left)
        return NULL;

    while (peek_token(cur) &&
           (peek_token(cur)->tokentype == TOK_AND || peek_token(cur)->tokentype == TOK_OR))
    {
        t_lexer *op = consume_token(cur);
        ast *right = parse_pipeline(cur);
        if (!right)
        {
            free_ast(left);
            return NULL;
        }
        ast *node = calloc(1, sizeof(ast));
        if (!node)
        {
            free_ast(left);
            free_ast(right);
            return NULL;
        }
        node->type = (op->tokentype == TOK_AND ? NODE_AND : NODE_OR);
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

ast *parse_cmdline(t_lexer **cur)
{
    ast *root = parse_and_or(cur);
    if (!root)
        return NULL;

    t_lexer *pt = peek_token(cur);
    if (pt && pt->tokentype != TOK_END)
    {
        fprintf(stderr, "Syntax error: unexpected token at end (type %d)\n", pt->tokentype);
        free_ast(root);
        return NULL;
    }
    return root;
}

/* ---------- AST / Token memory release ---------- */

static void free_ast(ast *node)
{
    if (!node)
        return;
    switch (node->type)
    {
    case NODE_CMD:
        if (node->argv)
        {
            for (int i = 0; node->argv[i]; i++)
            {
                free(node->argv[i]);
            }
            free(node->argv);
        }
        free(node->redir_in);
        free(node->redir_out);
        free(node->redir_append);
        free(node->heredoc_delim);
        break;
    case NODE_PIPE:
    case NODE_AND:
    case NODE_OR:
        free_ast(node->left);
        free_ast(node->right);
        break;
    case NODE_SUBSHELL:
        free_ast(node->sub);
        break;
    default:
        break;
    }
    free(node);
}

static void free_tokens(t_lexer *tok)
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

static void print_indent(int depth)
{
    for (int i = 0; i < depth; i++)
        printf("  "); // 每层两个空格缩进
}

void print_ast(ast *node, int depth)
{
    if (!node)
        return;

    print_indent(depth);

    switch (node->type)
    {
    case NODE_CMD:
        printf("CMD");
        for (size_t i = 0; node->argv && node->argv[i]; i++)
            printf(" \"%s\"", node->argv[i]);
        if (node->redir_in)
            printf(" < %s", node->redir_in);
        if (node->redir_out)
            printf(" > %s", node->redir_out);
        if (node->redir_append)
            printf(" >> %s", node->redir_append);
        if (node->heredoc_delim)
            printf(" << %s", node->heredoc_delim);
        printf("\n");
        break;

    case NODE_PIPE:
        printf("PIPE\n");
        print_ast(node->left, depth + 1);
        print_ast(node->right, depth + 1);
        break;

    case NODE_AND:
        printf("AND\n");
        print_ast(node->left, depth + 1);
        print_ast(node->right, depth + 1);
        break;

    case NODE_OR:
        printf("OR\n");
        print_ast(node->left, depth + 1);
        print_ast(node->right, depth + 1);
        break;

    case NODE_SUBSHELL:
        printf("SUBSHELL\n");
        print_ast(node->sub, depth + 1);
        break;

    default:
        print_indent(depth);
        printf("UNKNOWN NODE TYPE %d\n", node->type);
        break;
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    char buf[1024];

    printf("Enter a shell command:\n");
    if (!fgets(buf, sizeof(buf), stdin))
    {
        return 0;
    }
    /* strip newline */
    buf[strcspn(buf, "\n")] = '\0';

    t_minishell *general = calloc(1, sizeof(t_minishell));
    general->raw_line = buf;
    if (!general)
        return 1;
    if (handle_lexer(general))
    {
        printf("Lexer tokens:\n");
        print_lexer(general->lexer);
    }
    if (!general->lexer)
    {
        fprintf(stderr, "tokenize failed\n");
        return 1;
    }

    /* parser now uses a cursor pointer */
    t_lexer *cursor = general->lexer;
    ast *root = parse_cmdline(&cursor);
    if (root)
    {
        printf("=== AST ===\n");
        print_ast(root, 0);
        free_ast(root);
    }
    else
    {
        fprintf(stderr, "Parsing failed.\n");
    }

    free_tokens(general->lexer);

    return 0;
}
