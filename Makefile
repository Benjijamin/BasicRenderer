VULKANSDK=C:/VulkanSDK/1.3.290.0
WORKDIR=$(CURDIR)

VULKANHEADERS=$(VULKANSDK)/Include
GLFWINCLUDE=$(WORKDIR)/GLFW/include
GLMINCLUDE=$(WORKDIR)/glm
UTILSINCLUDE=$(WORKDIR)/utils
VKINCLUDE=$(WORKDIR)/vulkan
INCLUDES=-I$(WORKDIR)/ -I$(UTILSINCLUDE) -I$(VKINCLUDE) -I$(GLFWINCLUDE) -I$(GLMINCLUDE) -I$(VULKANHEADERS) 

VULKANLIB=$(VULKANSDK)/Lib
GLFWLIB=$(WORKDIR)/GLFW/lib
LINKS=-L$(GLFWLIB) -L$(VULKANLIB)

UTILS := $(wildcard $(WORKDIR)/utils/*.cpp)
UTILS_OBJECTS := $(UTILS:.cpp=.o)
VULKAN := $(wildcard $(WORKDIR)/vulkan/*.cpp)
VULKAN_OBJECTS := $(VULKAN:.cpp=.o)

OBJECTS := $(UTILS_OBJECTS) $(VULKAN_OBJECTS)

export INCLUDES
export VULKANSDK

main.exe: main.cpp
	@echo making utils
	@make -C utils
	@echo making shaders
	@make -C shaders
	@echo making vulkan
	@make -C vulkan
	@echo making exe
	@g++ -g $(INCLUDES) $(LINKS) $(OBJECTS) main.cpp -lglfw3 -lgdi32 -lvulkan-1 -o $(WORKDIR)/build/main.exe
	@echo build finished