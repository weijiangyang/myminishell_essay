# —————————————— 项目配置 ——————————————

# 可执行文件名称
NAME = minishell

# 路径定义
INCDIR = include
SRCDIR = src
LIBFTDIR = libft

# libft 静态库
LIBFT = $(LIBFTDIR)/libft.a

# 编译器 & 编译选项
CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(INCDIR) -I$(LIBFTDIR)

# 查找所有源文件（递归子目录）
SRC = $(shell find $(SRCDIR) -type f -name "*.c")
OBJ = $(SRC:.c=.o)

# —————————————— 规则 ——————————————

all: $(LIBFT) $(NAME)

$(LIBFT):
	@make -C $(LIBFTDIR)

$(NAME): $(OBJ)	
	$(CC) $(CFLAGS) $(OBJ) -L/opt/homebrew/opt/readline/lib -lreadline -o $(NAME)



# 单个 .c 到 .o 的规则
# 使用通配符 +自动创建目录
%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ)
	@make -C $(LIBFTDIR) clean

fclean: clean
	@rm -f $(NAME)
	@make -C $(LIBFTDIR) fclean

re: fclean all

.PHONY: all clean fclean re
