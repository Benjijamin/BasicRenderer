OBJS = vert.spv frag.spv
all: $(OBJS)

vert.spv: shader.vert
	$(info compiling vertex shader)
	glslc shader.vert -o vert.spv

frag.spv: shader.frag
	$(info compiling fragment shader)
	glslc shader.frag -o frag.spv