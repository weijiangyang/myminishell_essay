#include "../include/minishell.h"
#include "../libft/libft.h"

int main_2(void)
{
    t_env *env = NULL;
    char *argv1[] = {"export", "A=123", NULL};
    builtin_export(argv1, &env);
    // 测试 unset
    char *argv2[] = {"unset", "A", NULL};
    builtin_unset(argv2, &env);
    return 0;
}