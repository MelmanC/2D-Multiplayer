SRCSERVER = ./src/server/server.c ./src/packet.c \
						./src/server/utils_server.c

SRCCLIENT = ./src/client/client.c ./src/packet.c \
						./src/client/receive_packet.c \

CC = gcc

OBJ = $(SRCSERVER:.c=.o)
OBJCLIENT = $(SRCCLIENT:.c=.o)

NAME = server_exec
NAMECLIENT = client_exec

CFLAGS = -W -Werror -Wall -Wextra -I./headers -g

all: $(NAME) $(NAMECLIENT) clean

game: $(OBJCLIENT)
	$(CC) -o $(NAMECLIENT) $(OBJCLIENT)

server: $(OBJ)
	$(CC) -o $(NAME) $(OBJ)

$(NAME): server

$(NAMECLIENT): game

clean:
	find . -name "*~" -delete
	rm -f *.gcno
	rm -f *.gcda
	rm -f $(OBJ) $(OBJCLIENT)

fclean: clean
	rm -f $(NAME) $(NAMECLIENT)

re: fclean all

.PHONY: all clean fclean re game server
