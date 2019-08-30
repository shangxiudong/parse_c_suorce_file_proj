
TASK := main
OBJ := main.o Parse_c.o
CC := g++
CFLAGS := -c -std=c++11
RM := rm -f
.PHONY:all
all:$(TASK)

main: $(OBJ)
	@echo "link all object files"
	$(CC) $^ -o $@

%.o: %.cpp
	@echo "compile source file to object file"
	$(CC) $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	@echo "clean all task files and object files"
	$(RM) $(TASK) $(OBJ)
