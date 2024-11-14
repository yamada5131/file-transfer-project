CC = gcc
CFLAGS = -Wall -g -I. -I../common
LDFLAGS = -L../common
LIBS = -lcommon

SRCS = main.c client.c session.c command.c
OBJS = $(SRCS:.c=.o)

TARGET = client

all: $(TARGET)

$(TARGET): $(OBJS)
	$(MAKE) -C ../common
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	$(MAKE) -C ../common clean
