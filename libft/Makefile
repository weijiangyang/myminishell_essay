# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: weiyang <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/06 08:55:57 by weiyang           #+#    #+#              #
#    Updated: 2025/05/19 14:52:05 by weiyang          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a
FLAG = -Wall -Wextra -Werror -MMD
CC = cc
AR = ar
ARFLAGS = rcs
SRC = ft_atoi.c ft_isascii.c    ft_memcmp.c     ft_putendl_fd.c ft_strdup.c   ft_strlen.c ft_strtrim.c \
      ft_bzero.c      ft_isdigit.c    ft_memcpy.c     ft_putnbr_fd.c  ft_striteri.c   ft_strmapi.c    ft_substr.c \
      ft_calloc.c     ft_isprint.c    ft_memmove.c    ft_putstr_fd.c  ft_strjoin.c    ft_strncmp.c    ft_tolower.c \
      ft_isalnum.c    ft_itoa.c       ft_memset.c     ft_split.c      ft_strlcat.c    ft_strnstr.c    ft_toupper.c \
      ft_isalpha.c    ft_memchr.c     ft_putchar_fd.c ft_strchr.c     ft_strlcpy.c    ft_strrchr.c

BONUS_SRC = ft_lstadd_back_bonus.c ft_lstclear_bonus.c ft_lstiter_bonus.c ft_lstmap_bonus.c ft_lstsize_bonus.c \
            ft_lstadd_front_bonus.c ft_lstdelone_bonus.c ft_lstlast_bonus.c ft_lstnew_bonus.c

OBJ = $(SRC:.c=.o)
BONUS_OBJ = $(BONUS_SRC:.c=.o)

DEP = $(SRC:.c=.d)
BONUS_DEP = $(BONUS_SRC:.c=.d)

PATH_HEADER = ./

all: $(NAME)

$(NAME): $(OBJ)
	 $(AR) $(ARFLAGS) $(NAME) $^

bonus: $(OBJ) $(BONUS_OBJ)
	$(AR) $(ARFLAGS)  $(NAME) $(OBJ) $(BONUS_OBJ)


%.o:%.c
	$(CC) $(FLAG) -I$(PATH_HEADER) -c $< -o $@

-include $(DEP) $(BONUS_DEP)

clean:
	rm -f $(OBJ) $(BONUS_OBJ) $(DEP) $(BONUS_DEP)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
