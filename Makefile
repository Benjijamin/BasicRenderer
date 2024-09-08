VULKANSDK=C:/VulkanSDK/1.3.290.0/Include/vulkan
WORKDIR=F:/Cpp/BasicRenderer

VULKANHEADERS=$(VULKANSDK)/Include/vulkan
INCLUDES=-I$(WORKDIR)/ -I$(WORKDIR)/math -I$(WORKDIR)/utils -I$(WORKDIR)/GLFW/include -I$(VULKANSDK)

VULKANLIB=$(VULKANSDK)/Lib
GLFWLIB := $(WORKDIR)/GLFW/lib
LINKS := -L$(GLFWLIB) -L$(VULKANLIB)

MATH := $(wildcard $(WORKDIR)/math/*.cpp)
MATH_OBJECTS := $(MATH:.cpp=.o)
UTILS := $(wildcard $(WORKDIR)/utils/*.cpp)
UTILS_OBJECTS := $(UTILS:.cpp=.o)

OBJECTS := $(MATH_OBJECTS) $(UTILS_OBJECTS)

export INCLUDES

main.exe: main.cpp
	make -C math
	make -C	utils
	g++ -g $(INCLUDES) $(LINKS) $(OBJECTS) main.cpp -lglfw3 -lgdi32 -o build/main.exe