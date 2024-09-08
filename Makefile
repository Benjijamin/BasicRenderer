WORKDIR=F:/Cpp/BasicRenderer
INCLUDES=-I$(WORKDIR)/ -I$(WORKDIR)/math -I$(WORKDIR)/utils -I$(WORKDIR)/GLFW/include

MATH := $(wildcard $(WORKDIR)/math/*.cpp)
MATH_OBJECTS := $(MATH:.cpp=.o)
UTILS := $(wildcard $(WORKDIR)/utils/*.cpp)
UTILS_OBJECTS := $(UTILS:.cpp=.o)

OBJECTS := $(MATH_OBJECTS) $(UTILS_OBJECTS)

GLFW_LINK := $(WORKDIR)/GLFW/lib

main.exe: main.cpp
	make -C math INCLUDES=$(INCLUDES)
	make -C	utils INCLUDES=$(INCLUDES)
	g++ -g $(INCLUDES) -L$(GLFW_LINK) $(OBJECTS) main.cpp -lglfw3 -lgdi32 -o build/main.exe