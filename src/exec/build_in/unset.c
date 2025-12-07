#include "../../../include/minishell.h"
#include "../../../libft//libft.h"

// 删除指定的环境变量
static void delete_env_var(t_env **env, const char *key)
{
    t_env *temp = *env;
    t_env *prev = NULL;

    // 如果链表为空
    if (!temp)
    {
        return;
    }

    // 如果要删除的是头节点
    if (strcmp(temp->key, key) == 0)
    {
        *env = temp->next; // 让头节点指向下一个节点
        free(temp->key);
        free(temp->value);
        free(temp);
        return;
    }

    // 否则遍历链表找到需要删除的节点
    while (temp != NULL && strcmp(temp->key, key) != 0)
    {
        prev = temp;
        temp = temp->next;
    }

    // 如果未找到该环境变量
    if (temp == NULL)
    {
        return;
    }

    // 删除该节点
    prev->next = temp->next;
    free(temp->key);
    free(temp->value);
    free(temp);
}

// `unset` 内建命令
int builtin_unset(char **argv, t_env **env)
{
    if (!argv[1])
    {
        fprintf(stderr, "unset: not enough arguments\n");
        return 1;
    }

    for (int i = 1; argv[i]; i++)
    {
        t_env *existing = find_env_var(*env, argv[i]);
        if (existing)
        {
            delete_env_var(env, argv[i]);
            printf("unset: `%s' has been removed.\n", argv[i]);
        }
        else
        {
            fprintf(stderr, "unset: `%s' not found.\n", argv[i]);
        }
    }
    return 0;
}