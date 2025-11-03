#include "../../include/minishell.h"
/**
 * @shell 的内置退出命令
 * 
 * @参数 args 不使用
 * @返回值： 1 (只为符合prototype)
 */
int    cell_exit(char **args)
{
    (void) args;
    exit(EXIT_SUCCESS);
    return (1);
}

/**
 * @用于打印当前进程的环境变量。通过访问外部变量 environ 来实现这一功能。
 * 
 * @参数： args （）
 * @返回值： 0 或 1；
 */
int cell_env(char **args)
{
    (void) args;
    extern  char **environ;

    int i;

    i = 0;
    
    if (!environ)
        return (1);
    while (environ[i])
    {
        printf("%s\n", environ[i]);
        i++;
    }
    return (0);
}
/**
 * @shell 内置echo 命令
 * 
 * @参数： 二维字符数组
 * @返回值： 1 或 0
 */
int cell_echo(char **args)
{
    int i = 1; // args[0] 通常是 "echo"

    if (!args)
        return (1);

    while (args[i])
    {
        printf("%s", args[i]);
        if (args[i + 1]) // 如果不是最后一个参数，输出空格
            printf(" ");
        i++;
    }
    printf("\n"); // 输出换行符
    return (0);
}