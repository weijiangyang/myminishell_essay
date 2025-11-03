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

#include "parse.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void free_ast(ast *node);
static void free_tokens(token *tok);



/* ---------- token helpers ---------- */

token *creat_new_token(tok_type ty, const char *txt)
{
    token *t = malloc(sizeof(token));
    if (!t)
        return NULL;
    t->type = ty;
    if (txt)
        t->text = strdup(txt);
    else
        t->text = NULL;
    t->next = NULL;
    return t;
}

int add_tok(token **header, token **tail, tok_type ty, const char *txt)
{
    if (header == NULL || tail == NULL)
        return 0;
    token *t = creat_new_token(ty, txt);
    if (!t)
        return 0;
    if (*header == NULL)
    {
        *header = t;
        *tail = t;
    }
    else
    {
        (*tail)->next = t;
        *tail = t;
    }
    return 1;
}

/* tokenize: convert a string into a token list (head pointer returned) */
static token *tokenize(const char *str)
{
    token *header = NULL;
    token *tail = NULL;
    const char *p = str;

    while (*p)
    {
        /* skip whitespace */
        if (isspace((unsigned char)*p))
        {
            p++;
            continue;
        }

        /* multi-char operators */
        if (p[0] == '&' && p[1] == '&')
        {
            add_tok(&header, &tail, TOK_AND, "&&");
            p += 2;
        }
        else if (p[0] == '|' && p[1] == '|')
        {
            add_tok(&header, &tail, TOK_OR, "||");
            p += 2;
        }
        else if (p[0] == '>' && p[1] == '>')
        {
            add_tok(&header, &tail, TOK_APPEND, ">>");
            p += 2;
        }
        else if (p[0] == '<' && p[1] == '<')
        {
            add_tok(&header, &tail, TOK_HEREDOC, "<<");
            p += 2;
        }
        else
        {
            /* single-char tokens or words */
            char c = *p;
            if (c == '|')
            {
                add_tok(&header, &tail, TOK_PIPE, "|");
                p++;
            }
            else if (c == '>')
            {
                add_tok(&header, &tail, TOK_REDIR_OUT, ">");
                p++;
            }
            else if (c == '<')
            {
                add_tok(&header, &tail, TOK_REDIR_IN, "<");
                p++;
            }
            else if (c == '(')
            {
                add_tok(&header, &tail, TOK_LPAREN, "(");
                p++;
            }
            else if (c == ')')
            {
                add_tok(&header, &tail, TOK_RPAREN, ")");
                p++;
            }
            else
            {
                const char *start = p;
                while (*p && !isspace((unsigned char)*p) && !(p[0] == '&' && p[1] == '&') && !(p[0] == '|' && p[1] == '|') && *p != '>' && *p != '<' && *p != '|' && *p != '(' && *p != ')')
                {
                    p++;
                }
                size_t len = (size_t)(p - start);
                char *buf = calloc(len + 1, 1);
                if (!buf)
                {
                    free_tokens(header); /* use the free below; but we haven't declared free_tokens yet -- in your project ensure it exists before use or handle differently */
                    return NULL;
                }
                strncpy(buf, start, len);
                add_tok(&header, &tail, TOK_WORD, buf);
                free(buf);
            }
        }
    }

    /* terminator */
    add_tok(&header, &tail, TOK_END, NULL);
    return header;
}

/* ---------- token cursor API (cursor is token **cur) ---------- */

/* peek at current token (does not advance) */
static token *peek_token(token **cur)
{
    if (!cur)
        return NULL;
    return *cur;
}

/* consume current token and advance cursor.
   returns the consumed token (old current) or NULL */
static token *consume_token(token **cur)
{
    if (!cur || !*cur)
        return NULL;
    token *old = *cur;
    *cur = (*cur)->next;
    return old;
}

/* ensure current token has the given type; consume it and return the consumed token.
   returns NULL on mismatch */
static token *expect_token(tok_type type, token **cur)
{
    if (!cur || !*cur || (*cur)->type != type)
    {
        fprintf(stderr, "Syntax error : expected token type %d\n", type);
        return NULL;
    }
    return consume_token(cur);
}

/* ---------- forward declarations of parse functions (they take token **cur) ---------- */
static ast *parse_and_or(token **cur);
static ast *parse_pipeline(token **cur);
static ast *parse_simple_cmd(token **cur);
static ast *parse_cmdline(token **cur);

/* ---------- parse implementations ---------- */

static ast *parse_simple_cmd(token **cur)
{
    token *t;
    token *pt = peek_token(cur);
    if (pt && pt->type == TOK_LPAREN)
    {
        /* subshell: consume "(" */
        consume_token(cur);
        ast *node = calloc(1, sizeof(ast));
        if (!node)
            return NULL;
        node->type = NODE_SUBSHELL;
        node->sub = parse_and_or(cur);
        /* expect ")" */
        if (!expect_token(TOK_RPAREN, cur))
        {
            fprintf(stderr, "Syntax error: expected ')'\n");
            free_ast(node);
            return NULL;
        }
        return node;
    }

    /* normal command */
    ast *node = calloc(1, sizeof(ast));
    if (!node)
        return NULL;
    node->type = NODE_CMD;

    size_t argv_cap = 8;
    size_t argc = 0;
    node->argv = calloc(argv_cap, sizeof(char *));
    if (!node->argv)
    {
        free(node);
        return NULL;
    }

    /* prefix redirections: while current token is a redirection */
    while ((pt = peek_token(cur)) &&
           (pt->type == TOK_REDIR_IN ||
            pt->type == TOK_REDIR_OUT ||
            pt->type == TOK_APPEND ||
            pt->type == TOK_HEREDOC))
    {
        token *redir = consume_token(cur);
        token *file = expect_token(TOK_WORD, cur);
        if (!file)
        {
            fprintf(stderr, "Syntax error: expected filename after redirection\n");
            free_ast(node);
            return NULL;
        }
        switch (redir->type)
        {
        case TOK_REDIR_IN:
            free(node->redir_in);
            node->redir_in = strdup(file->text);
            break;
        case TOK_REDIR_OUT:
            free(node->redir_out);
            node->redir_out = strdup(file->text);
            break;
        case TOK_APPEND:
            free(node->redir_append);
            node->redir_append = strdup(file->text);
            break;
        case TOK_HEREDOC:
            free(node->heredoc_delim);
            node->heredoc_delim = strdup(file->text);
            break;
        default:
            break;
        }
    }

    /* next token must be a word (command name) */
    pt = peek_token(cur);
    if (!pt || pt->type != TOK_WORD)
    {
        fprintf(stderr, "Syntax error: expected command name\n");
        free_ast(node);
        return NULL;
    }

    /* consume the command name */
    t = consume_token(cur);
    node->argv[argc++] = strdup(t->text);

    /* now loop over additional words and redirections */
    while ((pt = peek_token(cur)) &&
           (pt->type == TOK_WORD ||
            pt->type == TOK_REDIR_IN ||
            pt->type == TOK_REDIR_OUT ||
            pt->type == TOK_APPEND ||
            pt->type == TOK_HEREDOC))
    {
        if (pt->type == TOK_WORD)
        {
            t = consume_token(cur);
            if (argc + 1 >= argv_cap)
            {
                argv_cap *= 2;
                char **tmp = realloc(node->argv, argv_cap * sizeof(char *));
                if (!tmp)
                {
                    fprintf(stderr, "realloc failed\n");
                    free_ast(node);
                    return NULL;
                }
                node->argv = tmp;
            }
            node->argv[argc++] = strdup(t->text);
        }
        else
        {
            /* redirection */
            token *redir = consume_token(cur);
            token *file = expect_token(TOK_WORD, cur);
            if (!file)
            {
                fprintf(stderr, "Syntax error: expected filename after redirection\n");
                free_ast(node);
                return NULL;
            }
            switch (redir->type)
            {
            case TOK_REDIR_IN:
                free(node->redir_in);
                node->redir_in = strdup(file->text);
                break;
            case TOK_REDIR_OUT:
                free(node->redir_out);
                node->redir_out = strdup(file->text);
                break;
            case TOK_APPEND:
                free(node->redir_append);
                node->redir_append = strdup(file->text);
                break;
            case TOK_HEREDOC:
                free(node->heredoc_delim);
                node->heredoc_delim = strdup(file->text);
                break;
            default:
                break;
            }
        }
    }

    node->argv[argc] = NULL;
    return node;
}

static ast *parse_pipeline(token **cur)
{
    ast *left = parse_simple_cmd(cur);
    int n_pipes = 0;
    if (!left)
        return NULL;

    while (peek_token(cur) && peek_token(cur)->type == TOK_PIPE)
    {
        /* consume '|' */
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

static ast *parse_and_or(token **cur)
{
    ast *left = parse_pipeline(cur);
    if (!left)
        return NULL;

    while (peek_token(cur) &&
           (peek_token(cur)->type == TOK_AND || peek_token(cur)->type == TOK_OR))
    {
        token *op = consume_token(cur);
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
        node->type = (op->type == TOK_AND ? NODE_AND : NODE_OR);
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

static ast *parse_cmdline(token **cur)
{
    ast *root = parse_and_or(cur);
    if (!root)
        return NULL;

    token *pt = peek_token(cur);
    if (pt && pt->type != TOK_END)
    {
        fprintf(stderr, "Syntax error: unexpected token at end (type %d)\n", pt->type);
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

static void free_tokens(token *tok)
{
    while (tok)
    {
        token *nx = tok->next;
        if (tok->text)
            free(tok->text);
        free(tok);
        tok = nx;
    }
}

static void print_ast(ast *node, int indent)
{
    if (!node)
        return;
    for (int i = 0; i < indent; i++)
        putchar(' ');
    switch (node->type)
    {
    case NODE_CMD:
        printf("CMD");
        if (node->argv)
        {
            for (int i = 0; node->argv[i]; i++)
            {
                printf(" \"%s\"", node->argv[i]);
            }
        }
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
        print_ast(node->left, indent + 2);
        print_ast(node->right, indent + 2);
        break;
    case NODE_AND:
        printf("AND\n");
        print_ast(node->left, indent + 2);
        print_ast(node->right, indent + 2);
        break;
    case NODE_OR:
        printf("OR\n");
        print_ast(node->left, indent + 2);
        print_ast(node->right, indent + 2);
        break;
    case NODE_SUBSHELL:
        printf("SUBSHELL\n");
        print_ast(node->sub, indent + 2);
        break;
    default:
        printf("UNKNOWN NODE\n");
        break;
    }
}

int main(int argc, char *argv[])
{
    char buf[1024];

    printf("Enter a shell command:\n");
    if (!fgets(buf, sizeof(buf), stdin))
    {
        return 0;
    }
    /* strip newline */
    buf[strcspn(buf, "\n")] = '\0';

    token *tok = tokenize(buf);
    if (!tok)
    {
        fprintf(stderr, "tokenize failed\n");
        return 1;
    }

    /* parser now uses a cursor pointer */
    token *cursor = tok;
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

    free_tokens(tok);
    
    return 0;
}
