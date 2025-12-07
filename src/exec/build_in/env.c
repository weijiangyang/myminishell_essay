#include "../../../include/minishell.h"
#include "../../../libft//libft.h"

void    print_env(t_env *env)
{
    while (env)
    {
        if (env->value)  // n'afficher que KEY=VALUE
        {
            printf("%s=%s\n", env->key, env->value);
        }
        env = env->next;
    }
}

t_env	*env_new(char *key, char *value)
{
	t_env	*new_env;

	new_env = (t_env *)malloc(sizeof(t_env));
	if (!new_env)
		return (NULL);
	new_env->key = key;
    new_env->value = value;
	new_env->next = NULL;
	return (new_env);
}

void	env_add_back(t_env **env, t_env *new_env)
{
	t_env	*last;

	if (!env || !new_env)
		return ;
	if (*env == NULL)
	{
		*env = new_env;
		return ;
	}
	last = *env;
	while (last->next)
		last = last->next;
	last->next = new_env;
}

t_env *init_env(char **envp)
{
    t_env *env;

    env = NULL;
    int     i = 0;

    while (envp[i])
    {
        char *equal = strchr(envp[i], '=');
        if (equal)
        {
            char *key = strndup(envp[i], equal - envp[i]);
            char *value = strdup(equal + 1);
            env_add_back(&env, env_new(key, value));
        }
        i++;
    }
    return env;
}

int builtin_env(char **argv, t_env *env)
{
    
    if (argv[1])  // si des arguments → erreur
    {
        fprintf(stderr, "env: %s: No such file or directory\n", argv[1]);
        return 127; // code d’erreur bash
    }
    print_env(env);
    return 0;
}

