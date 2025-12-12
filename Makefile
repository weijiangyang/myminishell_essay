# —————————————— 项目配置 ——————————————
NAME = minishell
INCDIR = include
SRCDIR = src
LIBFTDIR = libft
BUILD = build

LIBFT = $(LIBFTDIR)/libft.a
CC = cc

# 添加 readline 路径（Homebrew 通常安装在 /opt/homebrew）
READLINE_INC = /opt/homebrew/opt/readline/include
READLINE_LIB = /opt/homebrew/opt/readline/lib

CFLAGS = -Wall -Wextra -Werror \
         -I$(INCDIR) \
         -I$(LIBFTDIR) \
         -I$(READLINE_INC)

LDFLAGS = -L$(READLINE_LIB)
LDLIBS = -lreadline

# 查找所有源文件
SRC = $(shell find $(SRCDIR) -type f -name "*.c")

# 将 src/*.c 转换为 build/*.o
OBJ = $(patsubst $(SRCDIR)/%.c,$(BUILD)/%.o,$(SRC))

# —————————————— 规则 ——————————————

all: $(LIBFT) $(NAME)

$(LIBFT):
	@make -C $(LIBFTDIR) bonus

# 链接 minishell
$(NAME): $(OBJ) $(LIBFT)
	$(CC) $(LDFLAGS) $(OBJ) $(LIBFT) $(LDLIBS) -o $(NAME)

# 单文件编译规则：build/ 目录自动创建
$(BUILD)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	@rm -rf $(BUILD)
	@make -C $(LIBFTDIR) clean

fclean:
	@rm -rf $(BUILD)
	@rm -f $(NAME) a.out
	@make -C $(LIBFTDIR) fclean

re: fclean all

.PHONY: all clean fclean re
