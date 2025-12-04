#include "../../../include/minishell.h"
#include "../../../libft//libft.h"

// 这是一个简化的实现，只处理一些常见的转义序列
static void print_with_escape(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\\' && str[i + 1] != '\0')
        {
            i++; // 移动到反斜杠后面的字符
            switch (str[i])
            {
            case 'n':
                printf("\n");
                break;
            case 't':
                printf("\t");
                break;
            case 'a':
                printf("\a");
                break;
            case '\\':
                printf("\\");
                break;
            // 可以添加更多转义符，例如 'r', 'v', 'b', 八进制/十六进制等
            default:
                // 如果不是识别的转义字符，则打印反斜杠和该字符
                printf("\\%c", str[i]);
                break;
            }
        }
        else
        {
            // 正常字符，直接打印
            printf("%c", str[i]);
        }
    }
}

// 核心 echo 命令处理函数
int ft_echo(char **argv)
{
    int i = 1;
    int print_newline = 1; // 默认打印换行符
    int enable_escape = 0; // 默认不启用转义

    // 1. 解析选项 (-n 和 -e)
    while (argv[i] && (ft_strncmp(argv[i], "-n", 2) == 0 || ft_strncmp(argv[i], "-e", 2) == 0))
    {
        if (ft_strncmp(argv[i], "-n", 2) == 0 && argv[i][2] == '\0')
        {
            print_newline = 0;
        }
        else if (ft_strncmp(argv[i], "-e", 2) == 0 && argv[i][2] == '\0')
        {
            enable_escape = 1;
        }
        // *注意*: 许多 shell 会处理组合选项如 '-ne' 或连续选项 '-n -e'。
        // 这里的简化实现只处理独立的 "-n" 和 "-e" 选项。

        i++; // 移动到下一个参数
    }

    // 2. 打印参数
    while (argv[i])
    {
        if (enable_escape)
        {
            print_with_escape(argv[i]);
        }
        else
        {
            printf("%s", argv[i]);
        }

        // 在参数之间打印空格
        if (argv[i + 1])
        {
            printf(" ");
        }
        i++;
    }

    // 3. 打印换行符 (如果未禁用)
    if (print_newline)
    {
        printf("\n");
    }

    return 0; // 成功返回 0
}

