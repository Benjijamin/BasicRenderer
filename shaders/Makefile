OBJS = vert.spv frag.spv
all: $(OBJS)

vert.spv: shader.vert
	$(info compiling vertex shader)
	$(VULKANSDK)/bin/glslc shader.vert -o vert.spv

frag.spv: shader.frag
	$(info compiling fragment shader)
	$(VULKANSDK)/bin/glslc shader.frag -o frag.spv