OBJS = vkstructs.o vkdebug.o vkvertex.o vkhelpers.o
all: $(OBJS)

vkstructs.o: vkstructs.cpp vkstructs.h
	$(info making vkstructs)
	g++ -c $(INCLUDES) vkstructs.cpp -o vkstructs.o

vkdebug.o: vkdebug.cpp vkdebug.h
	$(info making vkdebug)
	g++ -c $(INCLUDES) vkdebug.cpp -o vkdebug.o

vkvertex.o: vkvertex.cpp vkvertex.h
	$(info making vkvertex)
	g++ -c $(INCLUDES) vkvertex.cpp -o vkvertex.o

vkhelpers.o: vkhelpers.cpp vkhelpers.h
	$(info making vkhelpers)
	g++ -c $(INCLUDES) vkhelpers.cpp -o vkhelpers.o