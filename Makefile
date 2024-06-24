CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -pthread

COMMON_SRC = src/server.c src/user.c src/encryption.c src/logger.c
COMMON_OBJ = $(COMMON_SRC:.c=.o)

MPCC_SRC = src/main.c src/client.c
MPCC_OBJ = $(MPCC_SRC:.c=.o) $(COMMON_OBJ)
MPCC_TARGET = mpcc

MAIN_SERVER_SRC = src/main_server.c
MAIN_SERVER_OBJ = $(MAIN_SERVER_SRC:.c=.o) $(COMMON_OBJ)
MAIN_SERVER_TARGET = main_server

.PHONY: all clean

all: $(MPCC_TARGET) $(MAIN_SERVER_TARGET)

$(MPCC_TARGET): $(MPCC_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(MAIN_SERVER_TARGET): $(MAIN_SERVER_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(MPCC_TARGET) $(MAIN_SERVER_TARGET)
