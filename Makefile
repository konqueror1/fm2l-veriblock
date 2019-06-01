# Change this path if the SDK was installed in a non-standard location
OPENCL_HEADERS = "/opt/AMDAPPSDK-3.0/include"
# By default libOpenCL.so is searched in default system locations, this path
# lets you adds one more directory to the search path.
LIBOPENCL = "/opt/amdgpu-pro/lib/x86_64-linux-gnu"

CC = g++
CPPFLAGS = -I${OPENCL_HEADERS} -I./incs
CFLAGS = -O2 -Wall
# LDFLAGS =  -pthread -lpthread -rdynamic -L${LIBOPENCL} -fpermissive
LDFLAGS =  -pthread -lpthread  -L./libs -L${LIBOPENCL} -fpermissive
LDLIBS = -lOpenCL -lrt
OBJ = kernel.cc libs/rs232-linux.c
INCLUDES = _kernel.h
EXE = veri_fm2l
all : ${EXE}

${EXE} : ${OBJ} 
	${CC} -std=gnu++11 -o ${EXE} ${OBJ} ${LDFLAGS} 

${OBJ} : ${INCLUDES} UCPClient.h Miner.h picojson.h

_kernel.h : input.cl 
	echo 'const char *ocl_code = R"_mrb_(' >$@
	cpp $< >>$@
	echo ')_mrb_";' >>$@

clean :
	rm -f ${EXE} _kernel.h *.o libs/*.o _temp_*

re : clean all
