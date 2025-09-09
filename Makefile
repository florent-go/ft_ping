NAME = ft_ping

SRC_DIR = srcs
INC_DIR = includes

SRC = $(SRC_DIR)/main.c \
      $(SRC_DIR)/check.c \
	  $(SRC_DIR)/print.c \
	  $(SRC_DIR)/socket.c \
	  $(SRC_DIR)/icmp.c \
	  $(SRC_DIR)/utils.c

OBJ = $(SRC:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -I$(INC_DIR)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re