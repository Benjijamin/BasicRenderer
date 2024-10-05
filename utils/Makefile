OBJS = utils.o

all: $(OBJS)

utils.o: utils.cpp utils.h
	$(info making utils)
	g++ -c $(INCLUDES) utils.cpp -o utils.o