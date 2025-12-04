#ifndef EXEC_H
#define EXEC_H

int exec_ast(ast *n);
int exec_builtin(ast *node);
int is_builtin(const char *cmd);
int ft_cd(char **argv);
int ft_echo(char **argv);
#endif