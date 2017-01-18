#Compiler settings

CC = g++
CFLAGS = -c -Wall -g -O2
SRCS = main.cpp buffer.cpp components.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = exec
SCRIPT = insert_unitest.script
	
all : $(SRCS) $(EXE)
	@echo $(EXE) "HAS BEEN COMPILED" 
	
$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
script :
	chmod +x $(SCRIPT)

clean :
	@echo "Removing files"
	rm $(EXE) *.o

rebuild :
	make clean
	make

cleanTXT :
	rm REVERSEDSorted.txt newSorted.txt graphINCOMING_REVERSED.txt graphOUTCOMING.txt results.txt
