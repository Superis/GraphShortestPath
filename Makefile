#Compiler settings

CC = g++
CFLAGS = -c -Wall -g 
SRCS = main.cpp Buffer.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = exec

	
all : $(SRCS) $(EXE)
	@echo $(EXE) "HAS BEEN COMPILED" 
	
$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean :
	@echo "Removing files"
	rm $(EXE) *.o
	
