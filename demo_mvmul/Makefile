C++ = $(CROSS)g++
## seems no need to define arch=IA32/IA64/POWERPC/AMD64

CCFLAGS = $(INCLUDE) -Wall -fPIC -finline-functions -fno-strict-aliasing 
CCFLAGS += -fvisibility=hidden -fdata-sections -ffunction-sections -Os

LDFLAGS = -lstdc++ -lpthread


DIR = $(shell pwd)

APP = demo_mvmul

all: $(APP)

%.o: %.cpp
	$(C++) $(CCFLAGS) $< -c -o $@

demo_mvmul: demo_mvmul.o
	$(C++) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o $(APP)

