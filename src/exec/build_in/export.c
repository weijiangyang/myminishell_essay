#include "../../../include/minishell.h"
#include "../../../libft//libft.h"

// 查找环境变量是否存在
t_env *find_env_var(t_env *env, const char *key) {
    while (env) {
        if (strcmp(env->key, key) == 0)
            return env;
        env = env->next;
    }
    return NULL;
}

// 判断是否是合法的环境变量名
int is_valid_identifier(const char *str) {
    if (!str || !isalpha(str[0])) {
        return 0;  // 环境变量名必须以字母开头
    }
    for (int i = 0; str[i]; i++) {
        if (!isalnum(str[i]) && str[i] != '_') {
            return 0;  // 环境变量名只能包含字母、数字和下划线
        }
    }
    return 1;
}

int builtin_export(char **argv, t_env **env) {
    if (argv[1] == NULL) {
        // 如果没有参数，打印所有的 export 环境变量
        print_env(*env);  // 假设 print_env 函数已定义
        return 0;
    }

    for (int i = 1; argv[i]; i++) {
        // 查找 '=' 字符
        char *equal = strchr(argv[i], '=');

        // 如果 '=' 存在，提取键和值
        if (equal) {
            // 提取键名部分
            char *key = strndup(argv[i], equal - argv[i]);

            // 检查键名是否合法
            if (!is_valid_identifier(key)) {
                fprintf(stderr, "export: `%s': not a valid identifier\n", argv[i]);
                free(key);
                return 1;
            }

            // 提取值部分
            char *value = strdup(equal + 1);
            if (!key || !value) {
                perror("Memory allocation failed");
                return 1;  // 处理内存分配失败的情况
            }

            // 查找是否已存在该环境变量
            t_env *existing = find_env_var(*env, key);
            if (existing) {
                // 如果已经存在，更新值
                free(existing->value);
                existing->value = value;
                free(key);
            } else {
                // 如果不存在，创建新变量
                env_add_back(env, env_new(key, value));  // 假设 env_add_back 和 env_new 已定义
            }
        } else {
            // 如果没有 '='，说明是一个没有值的环境变量（仅键名）
            char *key = strdup(argv[i]);
            if (!key) {
                perror("Memory allocation failed");
                return 1;  // 处理内存分配失败的情况
            }

            // 检查键名是否合法
            if (!is_valid_identifier(key)) {
                fprintf(stderr, "export: `%s': not a valid identifier\n", argv[i]);
                free(key);
                return 1;
            }

            // 查找是否已存在该环境变量
            t_env *existing = find_env_var(*env, key);
            if (existing) {
                free(key);  // 如果已存在，释放临时键
            } else {
                // 如果不存在，创建新变量，值为空字符串
                env_add_back(env, env_new(key, ""));
            }
        }
    }
    return 0;
}
