VULKANSDK=C:/VulkanSDK/1.3.290.0
WORKDIR=$(CURDIR)

VULKANHEADERS=$(VULKANSDK)/Include
GLFWINCLUDE=$(WORKDIR)/GLFW/include
MATHINCLUDE=$(WORKDIR)/math
UTILSINCLUDE=$(WORKDIR)/utils
INCLUDES=-I$(WORKDIR)/ -I$(MATHINCLUDE) -I$(UTILSINCLUDE) -I$(GLFWINCLUDE) -I$(VULKANHEADERS) 

VULKANLIB=$(VULKANSDK)/Lib
GLFWLIB=$(WORKDIR)/GLFW/lib
LINKS=-L$(GLFWLIB) -L$(VULKANLIB)

MATH := $(wildcard $(WORKDIR)/math/*.cpp)
MATH_OBJECTS := $(MATH:.cpp=.o)
UTILS := $(wildcard $(WORKDIR)/utils/*.cpp)
UTILS_OBJECTS := $(UTILS:.cpp=.o)

OBJECTS := $(MATH_OBJECTS) $(UTILS_OBJECTS)

export INCLUDES

main.exe: main.cpp
	make -C math
	make -C	utils
	make -C shaders
	$(info making exe)
	g++ -g $(INCLUDES) $(LINKS) $(OBJECTS) main.cpp -lglfw3 -lgdi32 -lvulkan-1 -o $(WORKDIR)/build/main.exe