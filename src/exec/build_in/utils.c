#include "../../../include/minishell.h"
#include "../../../libft//libft.h"

/**
 * change_envp - 将链表中的环境变量转换为一个数组，并更新 envp 指针。
 * 
 * 该函数将一个环境变量链表（t_env 类型）中的每个键值对转换为 `key=value` 格式的字符串，并将这些字符串存储到 `envp` 数组中。
 * 如果 `envp` 指针为空，则为其分配足够的内存来存储所有环境变量。最终，envp 数组将以 `NULL` 结尾，标志着数组的结束。
 * 
 * @env: 指向链表头的指针，该链表包含所有环境变量，每个环境变量由 t_env 结构体表示。
 * @envp: 指向 envp 数组的指针，该数组存储 `key=value` 格式的环境变量字符串。
 *        如果 envp 为空，该函数将为其分配内存。
 * 
 * 返回: 无返回值，直接更新传入的 envp 指针。
 */

void change_envp(t_env *env, char ***envp)
{
    int i = 0;
    t_env *tmp = env;

    // 计算链表中环境变量的数量
    while (tmp) {
        i++;
        tmp = tmp->next;
    }

    // 如果 envp 为空，进行内存分配
    if (*envp == NULL) {
        *envp = malloc(sizeof(char*) * (i + 1)); // 为 envp 分配内存
        if (*envp == NULL) {
            perror("malloc failed");
            return;
        }
    }

    tmp = env;
    i = 0;

    // 逐步分配内存，避免使用 realloc
    while (tmp) {
        // 拼接 key 和 value 字符串
        char *key_value = ft_strjoin(tmp->key, "=");
        if (key_value == NULL) {
            perror("strjoin failed");
            return;
        }
        char *env_str = ft_strjoin(key_value, tmp->value);
        free(key_value); // 释放中间拼接的字符串
        if (env_str == NULL) {
            perror("strjoin failed");
            return;
        }

        // 确保每次都分配新的内存
        (*envp)[i] = env_str;
        tmp = tmp->next;
        i++;
    }

    // 最后确保 envp 数组以 NULL 结尾
    (*envp)[i] = NULL;

    // 调试信息：打印结果
    //printf("envp array updated, number of items: %d\n", i);
    /*for (int j = 0; (*envp)[j] != NULL; j++) {
        printf("envp[%d]: %s\n", j, (*envp)[j]);
    }*/
}