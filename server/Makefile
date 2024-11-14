CC = gcc
CFLAGS = -Wall -g -I. -I../common
LDFLAGS = -L../common
LIBS = -lcommon -lpthread

SRCS = main.c server.c session.c handler.c data_manager.c
OBJS = $(SRCS:.c=.o)

TARGET = server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(MAKE) -C ../common
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	$(MAKE) -C ../common clean
