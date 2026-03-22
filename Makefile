CC = gcc
CFLAGS = -Wall -Wextra -g -pthread

TARGET_HYPHEN = cpd-terminal
TARGET_PLAIN = cpdterminal

OBJS = cpd-terminal.o commandlinereader/commandlinereader.o

all: $(TARGET_HYPHEN)

$(TARGET_HYPHEN): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_HYPHEN) $(OBJS)

$(TARGET_PLAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET_PLAIN) $(OBJS)

cpd-terminal.o: cpd-terminal.c commandlinereader/commandlinereader.h
	$(CC) $(CFLAGS) -c cpd-terminal.c

commandlinereader/commandlinereader.o: commandlinereader/commandlinereader.c commandlinereader/commandlinereader.h
	$(CC) $(CFLAGS) -c commandlinereader/commandlinereader.c -o commandlinereader/commandlinereader.o

clean:
	rm -f *.o commandlinereader/*.o $(TARGET_HYPHEN) $(TARGET_PLAIN)
