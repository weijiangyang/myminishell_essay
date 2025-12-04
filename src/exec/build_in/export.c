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

// 用来设置或更新环境变量
int builtin_export(char **argv, char **envp)
{
    t_env *env_1 = init_env(envp);
    if (argv[1] == NULL)
    {
        // 如果没有参数，打印所有的 export 环境变量
        print_env(env_1);
        return 0;
    }

    for (int i = 1; argv[i]; i++)
    {
        // 查找 '=' 字符
        char *equal = strchr(argv[i], '=');
        if (equal)
        {
            // 提取键和值
            char *key = strndup(argv[i], equal - argv[i]);
            char *value = strdup(equal + 1);

            // 查找是否已存在该环境变量
            t_env *existing = find_env_var(env_1, key);
            if (existing)
            {
                // 如果已经存在，更新值
                free(existing->value);
                existing->value = value;
            }
            else
            {
                // 如果不存在，创建新变量
                env_add_back(&env_1, env_new(key, value));
            }
            free(key);
        }
        else
        {
            fprintf(stderr, "export: `%s': not a valid identifier\n", argv[i]);
            return 1;
        }
    }
    return 0;
}