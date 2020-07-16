# Makefile of #1 exercise - Semester A (winter) 2019


CC = gcc
CFLAGS = -Wall  -lpthread
LINKFLAGS =

OBJECT1 = launcher.o
OBJECT2 = miner.o
OBJECT3 = server.o


SOURCE1 = launcher.c
SOURCE2 = miner.c
SOURCE3 = server.c


HEADER1 = launcher.h
HEADER2 = miner.h
HEADER3 = server.h

EXEFILE = launcher.out
MINER_OUT = miner.out
SERVER_OUT = server.out


all :  $(MINER_OUT) $(SERVER_OUT) $(EXEFILE)

server.out:blockList.o messageQueueList.o connectionRequest.h server.h server.c
	$(CC) $(CFLAGS) -o server.out server.c messageQueueList.o blockList.o -lrt -lz -lpthread
#	$(CC)  -o server.out $(CFLAGS) server.o -lrt

##server.o: blockList.o messageQueueList.o connectionRequest.h
	##$(CC) $(CFLAGS) -c server.c messageQueueList.o blockList.o
miner.out: miner.o
	$(CC)  -o miner.out $(CFLAGS) miner.o -lrt -lz

#server.c: blockList.o messageQueueList.o connectionRequest.h
	#$(CC) $(CFLAGS) -o server.c messageQueueList.o blockList.o

miner.o: miner.h miner.c connectionRequest.h
	$(CC) $(CFLAGS) -c miner.c

blockList.o: blockStruct.o  blockList.h blockList.c
	$(CC) $(CFLAGS) -c blockList.c

messageQueueList.o: messageQueueList.h messageQueueList.c
	$(CC) $(CFLAGS) -c messageQueueList.c

$(EXEFILE) : launcher.o
	 $(CC)  -o launcher.out $(CFLAGS) launcher.o -lrt

launcher.o : blockList.o launcher.h launcher.c
	$(CC) $(CFLAGS) -c launcher.c
test: all
		@echo "";
		@echo going to run the program now;
		@echo "";
	./launcher.out
clean:
	rm  -vf *.o *.out
