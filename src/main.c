#include "../include/minishell.h"
// 分割符
#define DEL " "
// 全局变量： 状态号
int status = 0;
/**
 * @用于打印提示词并读取用户输入的命令行
 *
 * @返回值：一个以 null 结尾的字符串
 */
char *cell_read_line(void)
{
    char cwd[BUFFER_SIZE];
    getcwd_wrapped(cwd, sizeof(cwd));
    // 显示提示符
    char *prompt = NULL;
    // 你可以把 cwd + "$>" 组合成一个 prompt 字符串
    asprintf(&prompt, "%s $> ", cwd);  // 或者自己手写拼接

    char *input = readline(prompt);
    free(prompt);

    if (!input) {
        // EOF 或错误
        printf("[EOF]\n");
        return NULL;
    }

    if (input[0] != '\0') {
        add_history(input);
    }
    // readline 返回的字符串末尾没有 \n，所以一般不需要去除
    return input;
}

/**
 * @将输入的字符串 line 按照指定的分隔符 DEL 拆分成多个子字符串，
 *   并将这些子字符串存储在一个动态分配的二维字符数组中
 *
 * @参数： char *line
 * @返回值： 二维字符数组（tokens)
 */
char **cell_split_line(char *line)
{
    char **tokens;
    size_t buffer_size;
    buffer_size = BUFFER_SIZE;
    tokens = malloc_wrapper(buffer_size * sizeof(*tokens));
    int position = 0;

    char *token = strtok(line, DEL);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= (int)buffer_size)
        {
            buffer_size *= 2;
            tokens = realloc_wrapper(tokens, buffer_size * sizeof(*tokens));
        }
        token = strtok(NULL, DEL);
    }
    tokens[position] = NULL;
    for (int i = 0; i < position; i++)
        printf("Token[%d]: %s\n", i, tokens[i]);
    return (tokens);
}
/**
 * @创建一个子进程， 并在子进程内执行execvp命令
 *
 * @参数： 二维字符数组
 * @返回值： 无
 */
void cell_launch(char **args)
{
    if (fork_wrapper() == 0)
    {
        execvp_wrapper(args[0], args);
    }
    else
        wait_wrapper(&status);
}
/**
 * @用于命令解析，判断输入的命令是否为内建命令，并在是内建命令时直接执行，否则通过 execve 启动外部程序。
 *
 * @参数： 二维字符数组
 * @返回值： 无
 */
void cell_exec(char **args)
{
    char *current;
    int i;

    i = 0;
    t_builtin g_builtin[] =
        {
            {.builtin_name = "echo", .foo = cell_echo},
            {.builtin_name = "env", .foo = cell_env},
            {.builtin_name = "exit", .foo = cell_exit},
            {.builtin_name = NULL, .foo = NULL}};
    while ((current = g_builtin[i].builtin_name))
    {
        if (!strcmp(current, args[0]))
        {
            g_builtin[i].foo(args);
            return;
        }
        else
            i++;
    }
    cell_launch(args);
}

int main(int ac, char **av)
{
    (void)ac;
    (void)av;
    // line → 用来保存从标准输入读取的一整行命令（比如 "ls -l"）
    // args → 将 line 按空格拆分成数组，用作程序参数(比如 “args“）
    char *line;
    char **args;

    // 循环从标准输入读取文本直到EOF 或没有输入时，循环终止， 解析出包含tokens的二维字符数组
    while ((line = cell_read_line()))
    {
        args = cell_split_line(line);
        //命令解析并执行
        cell_exec(args);
        //释放内存
        free(line);
        free(args);
    }
    return EXIT_SUCCESS;
}
