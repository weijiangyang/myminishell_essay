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

int is_valid_identifier(const char *s)
{
    int i = 0;

    if (!s || !s[0])
        return 0;

    if (!(ft_isalpha(s[0]) || s[0] == '_'))
        return 0;

    i = 1;
    while (s[i])
    {
        if (!(ft_isalnum(s[i]) || s[i] == '_'))
            return 0;
        i++;
    }
    return 1;
}


int builtin_unset(char **argv, t_env **env)
{
    int status = 0;

    // ⚠️ unset 无参数是合法的
    for (int i = 1; argv[i]; i++)
    {
        if (!is_valid_identifier(argv[i]))
        {
            fprintf(stderr,
                "unset: `%s': not a valid identifier\n",
                argv[i]);
            status = 1;
        }
        else
        {
            // 找不到变量也没关系
            delete_env_var(env, argv[i]);
        }
    }
    return status;
}
