#include "../../../include/minishell.h"
#include "../../../libft//libft.h"


// 设置或更新环境变量
void env_set(t_env **env, const char *key, const char *value)
{
    t_env *var = find_env_var(*env, key);

    if (var)
    {
        // 已存在 → 更新值
        free(var->value);
        var->value = strdup(value);
        if (!var->value)
        {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // 不存在 → 新建变量并加入链表
        char *new_key = strdup(key);
        char *new_value = strdup(value);
        if (!new_key || !new_value)
        {
            perror("strdup");
            free(new_key);
            free(new_value);
            exit(EXIT_FAILURE);
        }
        t_env *new_var = (t_env *)malloc(sizeof(t_env));
        if (!new_var)
        {
            perror("malloc");
            free(new_key);
            free(new_value);
            exit(EXIT_FAILURE);
        }
        new_var->key = new_key;
        new_var->value = new_value;
        new_var->next = NULL;

        // 添加到链表末尾
        if (!*env)
        {
            *env = new_var;
        }
        else
        {
            t_env *last = *env;
            while (last->next)
                last = last->next;
            last->next = new_var;
        }
    }
}


int ft_cd(char **argv, t_env **env)
{
    // 当另一个终端删除了当前文件夹时， 如何才能不崩溃 --等待中
    char cwd[4096];
    char *target;

    // 1. 处理无参数 → HOME
    if (!argv[1])
    {
        target = getenv("HOME");
        if (!target)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }
    else
    {
        target = argv[1];
    }

    // 2. 获取当前路径作为 OLDPWD
    if (!getcwd(cwd, sizeof(cwd)))
    {
        perror("getcwd");
        return 1;
    }

    // 3. 切换目录
    if (chdir(target) != 0)
    {
        fprintf(stderr, "bash: cd: %s: No such file or directory\n", target);
        return 1;
    }

    // 4. 更新 PWD 和 OLDPWD（假设你有 env_set 函数）
    env_set(env, "OLDPWD", cwd);
    env_set(env, "PWD", getcwd(cwd, sizeof(cwd)));

    return 0;
}
