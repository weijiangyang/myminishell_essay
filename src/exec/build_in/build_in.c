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
int exec_builtin(ast *node, char **envp)
{
    if (!node || !node->argv || !node->argv[0])
        return 1;

    // 在这里根据命令名执行
    if (ft_strncmp(node->argv[0], "cd", 2) == 0)
        ft_cd(node->argv);
    else if (ft_strncmp(node->argv[0], "echo", 4) == 0)
        ft_echo(node->argv);
    else if (strcmp(node->argv[0], "pwd") == 0)
    {
        char cwd[4096];
        if (getcwd(cwd, sizeof(cwd)))
        {
            printf("%s\n", cwd);
            return 0;
        }
        return 1;
    }
    else if (strcmp(node->argv[0], "export") == 0)
        builtin_export(node->argv, envp);

    else if (strcmp(node->argv[0], "env") == 0)
        builtin_env(node->argv, envp);
    else if (strcmp(node->argv[0], "exit") == 0)
    {
        int status = 0;
        if (node->argv[1])
            status = ft_atoi(node->argv[1]);
        exit(status);
    }
    // 其它内置命令类似处理
    return 1; // 未知内置
}
