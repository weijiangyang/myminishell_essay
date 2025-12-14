#include "../../../include/minishell.h"
#include "../../../libft//libft.h"

int builtin_pwd()
{
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
        return 0;
    }
    else
    {
        perror("pwd"); // 打印详细错误
        return 1;
    }
}