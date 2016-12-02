#Compiler settings

CC = g++
CFLAGS = -c -Wall -g 
SRCS = main.cpp buffer.cpp components.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = exec
SCRIPT = insert_unitest.script
	
all : $(SRCS) $(EXE)
	chmod +x $(SCRIPT)
	@echo $(EXE) "HAS BEEN COMPILED" 
	
$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean :
	@echo "Removing files"
	rm $(EXE) *.o
	
