#ifndef EXEC_H
#define EXEC_H

typedef struct s_env {
    char *key;
    char *value;
    struct s_env *next;
} t_env;

int exec_ast(ast *n, char **envp);
int exec_builtin(ast *node, char **envp);
int is_builtin(const char *cmd);
int ft_cd(char **argv);
int ft_echo(char **argv);
int builtin_env(char **argv, char **envp);
void    print_env(t_env *env);
t_env	*env_new(char *key, char *value);
void	env_add_back(t_env **env, t_env *new_env);
t_env *init_env(char **envp);
int builtin_export(char **argv, char **env);

#endif