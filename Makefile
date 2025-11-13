# —————————————— 项目配置 ——————————————
NAME = minishell
INCDIR = include
SRCDIR = src
LIBFTDIR = libft
BUILD = build

LIBFT = $(LIBFTDIR)/libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror -I$(INCDIR) -I$(LIBFTDIR)

# 查找所有源文件
SRC = $(shell find $(SRCDIR) -type f -name "*.c")
SRC += main.c

# 将 src/*.c 转换为 build/*.o
OBJ = $(patsubst $(SRCDIR)/%.c,$(BUILD)/%.o,$(SRC))

# —————————————— 规则 ——————————————

all: $(LIBFT) $(NAME)

$(LIBFT):
	@make -C $(LIBFTDIR)

# 链接 minishell
$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJ) $(LIBFT) -lreadline -o $(NAME)

# 单文件编译规则：build/ 目录自动创建
$(BUILD)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	@rm -rf $(BUILD)
	@make -C $(LIBFTDIR) clean

fclean: clean
	@rm -f $(NAME) a.out
	@make -C $(LIBFTDIR) fclean

re: fclean all

.PHONY: all clean fclean re
