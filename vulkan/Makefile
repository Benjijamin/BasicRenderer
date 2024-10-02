OBJS = vkstructs.o vkdebug.o
all: $(OBJS)

vkstructs.o: vkstructs.cpp vkstructs.h
	$(info making vkstructs)
	g++ -c $(INCLUDES) vkstructs.cpp -o vkstructs.o

vkdebug.o: vkdebug.cpp vkdebug.h
	$(info making vkdebug)
	g++ -c $(INCLUDES) vkdebug.cpp -o vkdebug.o