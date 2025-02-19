TARGET = GS

CC = gcc

CFLAGS = -Wall -Wextra -I.

SRCS = main.c aux.c player_def.c gameEngine.c protocols.c protocols_responses.c

$(TARGET): $(SRCS)
	clear
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

v: $(TARGET)
	./$(TARGET) -v

clean:
	rm -f $(TARGET)
	rm -R ./GAMES/*
	rm ./SCORES/*.txt

	clear

valgrind: $(TARGET)
	valgrind ./$(TARGET) 


