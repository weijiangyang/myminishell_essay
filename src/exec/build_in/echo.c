#include "../../../include/minishell.h"
#include "../../../libft//libft.h"

// 核心 echo 命令处理函数
int ft_echo(char **argv)
{
    //处理 echo -nnnnnn  hello 的情况
    int i = 1;
    int print_newline = 1; // 默认打印换行符

    // 1. 解析 -n 选项（只处理独立的 "-n"）
    while (argv[i] && strcmp(argv[i], "-n") == 0)
    {
        print_newline = 0; // 禁止打印换行
        i++;               // 移动到下一个参数
    }

    // 2. 打印参数
    while (argv[i])
    {
        printf("%s", argv[i]);
        if (argv[i + 1])
            printf(" "); // 参数之间加空格
        i++;
    }

    // 3. 打印换行符（如果没有 -n）
    if (print_newline)
        printf("\n");

    return 0; // 成功返回 0
}

