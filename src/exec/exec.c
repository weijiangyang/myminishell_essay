#include "../../include/minishell.h"

// 执行命令节点（fork + exec 或内建）
static int exec_cmd_node(ast *n)
{
    // 处理内建命令（如 cd, exit 等）
    if (n->argv && n->argv[0])
    {
        if (strcmp(n->argv[0], "cd") == 0)
        {
            const char *dir = n->argv[1] ? n->argv[1] : getenv("HOME");
            if (chdir(dir) < 0)
            {
                perror("cd");
                return 1;
            }
            return 0;
        }
        if (strcmp(n->argv[0], "exit") == 0)
        {
            exit(0);
        }
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }
    if (pid == 0)
    {
        // 子进程：设置重定向
        if (n->redir_in)
        {
            int fd = open(n->redir_in, O_RDONLY);
            if (fd < 0)
            {
                perror("open redir_in");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (n->redir_out)
        {
            int fd = open(n->redir_out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd < 0)
            {
                perror("open redir_out");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (n->redir_append)
        {
            int fd = open(n->redir_append, O_WRONLY | O_CREAT | O_APPEND, 0666);
            if (fd < 0)
            {
                perror("open redir_append");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        // HEREDOC 暂时不完全支持 — 你可以把 node->heredoc_delim 当作临时文件名处理

        execvp(n->argv[0], n->argv);
        perror("execvp");
        exit(1);
    }
    else
    {
        int status = 0;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else
        {
            return 1;
        }
    }
}

int exec_ast(ast *n)
{
    if (!n)
        return 0;
    switch (n->type)
    {
    case NODE_CMD:
        return exec_cmd_node(n);
    case NODE_PIPE:
    {
        // 管道：创建 pipe，然后 fork 两个子进程
        int pipefd[2];
        if (pipe(pipefd) < 0)
        {
            perror("pipe");
            return 1;
        }
        pid_t pid1 = fork();
        if (pid1 < 0)
        {
            perror("fork");
            return 1;
        }
        if (pid1 == 0)
        {
            // 左侧命令：输出端写入 pipe
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            int rc = exec_ast(n->left);
            exit(rc);
        }

        pid_t pid2 = fork();
        if (pid2 < 0)
        {
            perror("fork");
            return 1;
        }
        if (pid2 == 0)
        {
            // 右侧命令：从 pipe 读入
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            int rc = exec_ast(n->right);
            exit(rc);
        }

        // 父进程：关闭管道端点，等待子进程
        close(pipefd[0]);
        close(pipefd[1]);
        int status1, status2;
        waitpid(pid1, &status1, 0);
        waitpid(pid2, &status2, 0);
        // 这里我们简单返回右侧命令的状态
        if (WIFEXITED(status2))
            return WEXITSTATUS(status2);
        return 1;
    }
    case NODE_AND:
    {
        int st = exec_ast(n->left);
        if (st == 0)
        {
            return exec_ast(n->right);
        }
        else
        {
            return st;
        }
    }
    case NODE_OR:
    {
        int st = exec_ast(n->left);
        if (st != 0)
        {
            return exec_ast(n->right);
        }
        else
        {
            return st;
        }
    }
    case NODE_SEQUENCE:
        exec_ast(n->left);
        return exec_ast(n->right);

    case NODE_BACKGROUND:
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork for bg");
            return 1;
        }

        if (pid == 0)
        {
            // === 子进程逻辑 ===

            // 1️⃣ 脱离父进程组 —— 否则仍是前台组
            setpgid(0, 0);

            // 2️⃣ 忽略 Ctrl+C / Ctrl+\ 信号（后台不应被中断）
            signal(SIGINT, SIG_IGN);
            signal(SIGQUIT, SIG_IGN);

            // 3️⃣ 执行后台命令（左子树）
            exec_ast(n->left);
            exit(0);
        }
        else
        {
            // === 父进程逻辑 ===

            // 4️⃣ 打印后台任务提示（可选）
            printf("[bg pid %d]\n", pid);

            // 5️⃣ 不等待后台进程
            // 6️⃣ 继续执行右子树（前台命令）
            if (n->right)
                exec_ast(n->right);

            // 7️⃣ 返回前台控制权
            tcsetpgrp(STDIN_FILENO, getpgrp());
            return 0;
        }
    }
    case NODE_SUBSHELL:
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork for subshell");
            return 1;
        }
        if (pid == 0)
        {
            int rc = exec_ast(n->sub);
            exit(rc);
        }
        else
        {
            int status = 0;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                return WEXITSTATUS(status);
            return 1;
        }
    }
    default:
        fprintf(stderr, "Unknown AST node type %d\n", n->type);
        return 1;
    }
}