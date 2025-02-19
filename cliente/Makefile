TARGET = player

CC = gcc

CFLAGS = -Wall -Wextra -I.

SRCS = main.c aux.c cmds.c error_msgs.c

localhost = 127.0.1.1

$(TARGET): $(SRCS)
	clear
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)

run: $(TARGET)
	./$(TARGET)

dbg: $(TARGET)
	./$(TARGET) -n tejo.tecnico.ulisboa.pt -p 58011

clean:
	rm -f $(TARGET)
	rm sv_responses/*.txt
	clear

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

