#include "../../include/minishell.h"

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
int exec_builtin(ast *node)
{
    if (!node || !node->argv || !node->argv[0])
        return 1;

    // 在这里根据命令名执行
    if (ft_strncmp(node->argv[0], "cd", 2) == 0)
    {
        if (node->argv[1])
            return chdir(node->argv[1]) == 0 ? 0 : 1;
        else
        {
            char *home_path = getenv("HOME");
            if (home_path == NULL)
            {
                return 1;
            }
            else
            {
                return chdir(home_path) == 0 ? 0 : 1;
            }
        }
    }
    else if (ft_strncmp(node->argv[0], "echo", 4) == 0)
        /*{
            int i = 1;
            while (node->argv[i])
            {
                printf("%s", node->argv[i]);
                if (node->argv[i + 1])
                    printf(" ");
                i++;
            }
            printf("\n");
            return 0;
        }*/
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
    else if (strcmp(node->argv[0], "exit") == 0)
    {
        int status = 0;
        if (node->argv[1])
            status = atoi(node->argv[1]);
        exit(status);
    }
    // 其它内置命令类似处理
    return 1; // 未知内置
}
