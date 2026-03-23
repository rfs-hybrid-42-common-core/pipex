# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/17 03:42:32 by maaugust          #+#    #+#              #
#    Updated: 2026/03/23 14:00:22 by maaugust         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================ PROJECT FILE NAMES ============================ #
NAME           = pipex
B_NAME         = pipex_bonus

# ============================== COMPILER FLAGS ============================== #
CC             = cc
CFLAGS         = -Wall -Wextra -Werror
INCLUDES       = -Iincludes -Ilibft/includes
B_INCLUDES     = -Ibonus/includes -I./../libft/includes
RM             = rm -rf

# ================================== COLORS ================================== #
GREEN          := \033[32m
RED            := \033[31m
YELLOW         := \033[33m
CYAN           := \033[36m
RESET          := \033[0m
BOLD           := \033[1m

# ================================== LIBFT =================================== #
LIBFT_PATH     = ./libft
LIBFT_LIB      = $(LIBFT_PATH)/libft.a

# =============================== SOURCE FILES =============================== #
# Mandatory files
SRC_PATH       = ./srcs
SRC            = $(shell find $(SRC_PATH) -name '*.c')

# Bonus files
B_SRC_PATH     = ./bonus/srcs
B_SRC          = $(shell find $(B_SRC_PATH) -name '*.c')

# Object files
OBJ_PATH       = ./objs
OBJ            = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC))

B_OBJ_PATH     = ./bonus/objs
B_OBJ          = $(patsubst $(B_SRC_PATH)/%.c, $(B_OBJ_PATH)/%.o, $(B_SRC))

# ============================ COMPILATION RULES ============================= #
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@printf "$(CYAN)Compiling:$(RESET) $(YELLOW)$<$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(B_OBJ_PATH)/%.o: $(B_SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@printf "$(CYAN)Compiling:$(RESET) $(YELLOW)$<$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) $(B_INCLUDES) -c $< -o $@

# =============================== BUILD TARGETS ============================== #
all: $(NAME)

$(NAME): $(LIBFT_LIB) $(OBJ)
	@printf "$(GREEN)✔ Pipex mandatory objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(OBJ) $(LIBFT_LIB) -o $(NAME)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(NAME)$(RESET)\n"

bonus: $(B_NAME)

$(B_NAME): $(LIBFT_LIB) $(B_OBJ)
	@printf "$(GREEN)✔ Pipex bonus objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(B_OBJ) $(LIBFT_LIB) -o $(B_NAME)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(B_NAME)$(RESET)\n"

$(LIBFT_LIB):
	@printf "$(CYAN)→ Building Libft...$(RESET)\n"
	@$(MAKE) -C $(LIBFT_PATH) >/dev/null \
	  || { printf "$(RED)✖ Libft build failed!$(RESET)\n"; exit 1; }
	@printf "$(GREEN)✔ Libft built.$(RESET)\n"
	
# =============================== CLEAN TARGETS ============================== #
clean:
	@$(RM) $(OBJ_PATH) $(B_OBJ_PATH)
	@rmdir -p --ignore-fail-on-non-empty $(OBJ_PATH) $(B_OBJ_PATH) 2>/dev/null || true
	@$(MAKE) -C $(LIBFT_PATH) clean >/dev/null
	@printf "$(YELLOW)• Cleaned object files.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME) $(B_NAME)
	@$(MAKE) -C $(LIBFT_PATH) fclean >/dev/null
	@printf "$(RED)• Full clean complete.$(RESET)\n"

# ============================== REBUILD TARGETS ============================= #
re: fclean all

.PHONY: all bonus clean fclean re
