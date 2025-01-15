CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lssl -lcrypto

SRC = src/main.c src/ftp_client.c src/ftp_utils.c
OBJ = $(SRC:.c=.o)
TARGET = ftp_client

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
