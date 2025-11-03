#include "../../include/minishell.h"
/**
 * @获取当前工作地址， 并处理错误。 
 * 
 * @参数： char *buf ， size
 * @返回值： 无
 */
void getcwd_wrapped(char *buf, size_t size)
{
    if (getcwd(buf, size) == NULL)
        perror("getcwd failed");
}
/**
 * @用malloc 分配内存， 并处理错误。
 * 
 * @参数：  size 
 * @返回值： void* 
 */
void *malloc_wrapper(size_t size)
{
    void *ptr;

    if (size == 0)
        return (NULL);
    ptr = malloc(size);
    if (!ptr)
    {
        perror("Malloc failed");
        exit(EXIT_FAILURE);
    }
    return (ptr);
}
/**
 * @重新分配内存， 并处理错误
 * 
 * @参数： 原来的内存地址*ptr, 内存大小 size
 * @返回值： 新的内存地址 *new_ptr
 */
void *realloc_wrapper(void *ptr, size_t size)
{
    void *new_ptr;

    if (size == 0)
        return (NULL);
    new_ptr = realloc(ptr, size);
    if (!new_ptr)
    {
        perror("Realloc failed");
        exit(EXIT_FAILURE);
    }
    return (new_ptr);
}
/**
 * @创建一个子进程， 并处理错误
 * 
 * @返回值：  pid_t 
 */
pid_t fork_wrapper()
{
    pid_t pid;

    pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    return (pid);
}
/**
 * @在当前进程中执行一个命令， 并处理错误
 * 
 * @参数： args[0]，args
 * @返回值： 无
 */
void execvp_wrapper(const char *file, char *const av[])
{
    if (!file || !av)
    {
        perror("execvp: invalid arguments");
        exit(EXIT_FAILURE);
    }
    if (execvp(file, av) == -1)
    {
        perror("child processus failed");
        exit(EXIT_FAILURE);
    }
}
/**
 * @等待子进程结束， 并返回子进程的退出状态
 * 
 * @参数： status 是一个 int 类型的变量，用来 存储子进程的退出信息，不是直接的退出码，而是一个包含多个信息的整数，需要配合宏提取
 * @返回值： 子进程的退出码
 */
int wait_wrapper(int *status)
{
    pid_t result;

    if (!status)
    {
        perror("argument status is required");
        exit(EXIT_FAILURE);
    }
    result = wait(status);
    if (result == -1)
        printf("wait failed");
    if (WIFEXITED(*status))
        *status = WEXITSTATUS(*status);
    return (result);
}
