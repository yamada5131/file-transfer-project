CC = gcc
CFLAGS = -Wall -g -I.

# Danh sách các tệp nguồn và đối tượng
SRCS = protocol.c network.c cJSON.c
OBJS = $(SRCS:.c=.o)

LIB = libcommon.a

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs $(LIB) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(LIB)
