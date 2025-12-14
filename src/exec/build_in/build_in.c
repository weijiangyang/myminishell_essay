#include "../../../include/minishell.h"

// 检测命令是否为内置命令，返回 1 如果是，否则 0
int is_builtin(const char *cmd)
{
    if (!cmd)
        return 0;
    return (!ft_strncmp(cmd, "cd", 2) ||
            !ft_strncmp(cmd, "echo", 4) ||
            !ft_strncmp(cmd, "pwd", 3) ||
            !ft_strncmp(cmd, "exit", 4) ||
            !ft_strncmp(cmd, "export", 6) ||
            !ft_strncmp(cmd, "unset", 5) ||
            !ft_strncmp(cmd, "env", 3));
}

// 执行内置命令，返回退出码
int exec_builtin(ast *node, t_env **env)
{
    if (!node || !node->argv || !node->argv[0])
        return 1;

    // 在这里根据命令名执行
    if (ft_strncmp(node->argv[0], "cd", 2) == 0)
        return ft_cd(node->argv, env);
    else if (ft_strncmp(node->argv[0], "echo", 4) == 0)
        return ft_echo(node->argv);
    else if (strcmp(node->argv[0], "pwd") == 0)
        return builtin_pwd();
    else if (strcmp(node->argv[0], "export") == 0)
        return builtin_export(node->argv, env);

    else if (strcmp(node->argv[0], "env") == 0)
        return builtin_env(node->argv, *env);
    else if (strcmp(node->argv[0], "exit") == 0)
        return builtin_exit(node->argv);
    else if (strcmp(node->argv[0], "unset") == 0)
        builtin_unset(node->argv, env);
    // 其它内置命令类似处理
    return 1; // 未知内置
}
