#include "../../../include/minishell.h"
#include "../../../libft//libft.h"


static int is_numeric(const char *s)
{
    if (!s || !*s)
        return 0;

    int i = 0;
    if (s[0] == '+' || s[0] == '-')
        i++;

    for (; s[i]; i++)
    {
        if (!isdigit(s[i]))
            return 0;
    }
    return 1;
}

int builtin_exit(char **argv)
{
    //增加long_min, long_max 整数溢出的检测
    long status = 0;

    if (argv[1])
    {
        if (!is_numeric(argv[1]))
        {
            fprintf(stderr, "exit: %s: numeric argument required\n", argv[1]);
            exit(255);
        }

        status = atol(argv[1]); // 支持大数字
        if (argv[2])
        {
            fprintf(stderr, "exit: too many arguments\n");
            // Bash: 多参数时报错，但不退出 shell
            return (1);
        }
    }

    // exit code 只保留 0~255
    exit((unsigned char)status);
    return (0);
}

