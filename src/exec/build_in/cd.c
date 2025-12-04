#include "../../../include/minishell.h"
#include "../../../libft//libft.h"

int ft_cd(char **argv)
{
    if (argv[1])
    {

        int status = chdir(argv[1]);
        if (status == 0)
            return 0;
        else
            fprintf(stderr, "ce path n\'existe pas\n");
        return 1;
    }
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