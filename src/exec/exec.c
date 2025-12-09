#include "../../include/minishell.h"

int apply_redirs(t_redir *r)
{
    int fd;
    while (r)
    {
        if (r->type == REDIR_INPUT) // <
        {
            fd = open(r->filename, O_RDONLY);
            if (fd < 0)
            {
                perror(r->filename);
                return 1;
            }
            if (dup2(fd, STDIN_FILENO) < 0)
            {
                perror("dup2 infile");
                close(fd);
                return 1;
            }
            close(fd);
        }
        else if (r->type == REDIR_OUTPUT) // >
        {
            fd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror(r->filename);
                return 1;
            }
            if (dup2(fd, STDOUT_FILENO) < 0)
            {
                perror("dup2 outfile");
                close(fd);
                return 1;
            }
            close(fd);
        }
        else if (r->type == REDIR_APPEND) // >>
        {
            fd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0)
            {
                perror(r->filename);
                return 1;
            }
            if (dup2(fd, STDOUT_FILENO) < 0)
            {
                perror("dup2 append");
                close(fd);
                return 1;
            }
            close(fd);
        }
        else if (r->type == HEREDOC) // << (heredoc_fd 已保存为读端)
        {
            if (r->heredoc_fd < 0)
            {
                fprintf(stderr, "heredoc fd invalid\n");
                return 1;
            }
            if (dup2(r->heredoc_fd, STDIN_FILENO) < 0)
            {
                perror("dup2 heredoc");
                return 1;
            }
            close(r->heredoc_fd);
            r->heredoc_fd = -1;
        }
        r = r->next;
    }
    return 0;
}

static int apply_redirs_nocmd(t_redir *r)
{

    int fd;
    while (r)
    {
        if (r->type == REDIR_OUTPUT)
        {
            fd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror(r->filename);
                return 1;
            }

            close(fd);
        }
        else if (r->type == REDIR_APPEND)
        {
            fd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0)
            {
                perror(r->filename);
                return 1;
            }
            close(fd);
        }
        else if (r->type == REDIR_INPUT)
        {
            fd = open(r->filename, O_RDONLY);
            if (fd < 0)
            {
                perror(r->filename);
                return 1;
            }
            close(fd);
        }
        else if (r->type == HEREDOC)
        {
            if (r->heredoc_fd < 0)
            {
                fprintf(stderr, "heredoc fd invalid\n");
                return 1;
            }
            close(r->heredoc_fd);
            r->heredoc_fd = -1;
        }
        r = r->next;
    }
    return 0;
}

static void close_heredoc_fds(t_redir *r)
{
    while (r)
    {
        if (r->type == HEREDOC && r->heredoc_fd >= 0)
        {
            close(r->heredoc_fd);
            r->heredoc_fd = -1;
        }
        r = r->next;
    }
}

// 执行命令节点（fork + exec 或内建）
static int exec_cmd_node(ast *n, t_env **env)
{
    if (!n)
        return 1;

    // 纯重定向，没有命令
    // 纯重定向
    if (!n->argv && n->redir)
        return apply_redirs_nocmd(n->redir);
    if (is_builtin(n->argv[0]))
    {
        if (n->redir)
        {
            // 临时保存标准输入输出
            int stdin_bak = dup(STDIN_FILENO);
            int stdout_bak = dup(STDOUT_FILENO);
            if (apply_redirs(n->redir) < 0)
                return 1;
            int rc = exec_builtin(n, env);
            // 恢复标准输入输出
            dup2(stdin_bak, STDIN_FILENO);
            dup2(stdout_bak, STDOUT_FILENO);
            close(stdin_bak);
            close(stdout_bak);
            return rc;
        }
        else
            return exec_builtin(n, env);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        setup_child_signals();
        // child
        t_redir *r = n->redir;
        if (apply_redirs(r))
            exit(1);

        execvp(n->argv[0], n->argv);
        perror("execvp");
        exit(1);
    }
    else
    {
        //setup_parent_exec_signals();
        // parent
        // parent should close heredoc read fds
        close_heredoc_fds(n->redir);
        int status = 0;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        return 1;
    }
}

int exec_ast(ast *n, t_env **env)
{
    if (!n)
        return 0;
    switch (n->type)
    {
    case NODE_CMD:
        return exec_cmd_node(n, env);
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
            int rc = exec_ast(n->left, env);
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
            int rc = exec_ast(n->right, env);
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
            int rc = exec_ast(n->sub, env);
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