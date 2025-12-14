#include "../../../include/minishell.h"
#include "../../../libft//libft.h"

t_env *find_env_var(t_env *env, const char *key)
{
    while (env)
    {
        if (strcmp(env->key, key) == 0)
            return env;
        env = env->next;
    }
    return NULL;
}

void print_export(t_env *env)
{
    while (env)
    {
        printf("declare -x %s", env->key);
        if (env->value)
            printf("=\"%s\"", env->value);
        printf("\n");
        env = env->next;
    }
}

int builtin_export(char **argv, t_env **env)
{
    int status = 0;

    if (!argv[1])
    {
        print_export(*env);
        return 0;
    }

    for (int i = 1; argv[i]; i++)
    {
        char *key = NULL;
        char *value = NULL;
        char *equal = strchr(argv[i], '=');
        t_env *existing = NULL;

        if (equal)
        {
            key = strndup(argv[i], equal - argv[i]);
            value = strdup(equal + 1);
        }
        else
        {
            key = strdup(argv[i]);
            value = NULL;
        }

        if (!is_valid_identifier(key))
        {
            fprintf(stderr, "export: `%s': not a valid identifier\n", argv[i]);
            status = 1;
            free(key);
            free(value);
            continue; // 继续处理后续参数
        }

        existing = find_env_var(*env, key);
        if (existing)
        {
            if (value)
            {
                free(existing->value);
                existing->value = value;
            }
            free(key); // 这个 key 不需要了
        }
        else
        {
            env_add_back(env, env_new(key, value)); // 节点接管 key/value
        }
    }
    return status;
}

