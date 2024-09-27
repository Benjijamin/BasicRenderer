OBJS = utils.o ppm.o

all: $(OBJS)

utils.o: utils.cpp utils.h
	$(info making utils)
	g++ -c $(INCLUDES) utils.cpp -o utils.o

ppm.o: ppm.cpp ppm.h
	$(info making ppm)
	g++ -c $(INCLUDES) ppm.cpp -o ppm.o
