# 
# TCM-FM2L FPGA Miner for Veriblock vBlake2B
#
# (C) 2019 , trustcoinmining.com
# 
# License : GPL V3 
#

CC = g++
CPPFLAGS = -I./incs
CFLAGS = -O2 -Wall
LDFLAGS =  -pthread -lpthread -fpermissive
LDLIBS = -lpthread
OBJ = veri-fm2l.cpp libs/rs232-linux.c
EXE = veri_fm2l
all : ${EXE}

${EXE} : ${OBJ} 
	${CC} -std=gnu++11 -o ${EXE} ${OBJ} ${CFLAGS} ${CPPFLAGS} ${LDFLAGS} 

${OBJ} : ${INCLUDES} UCPClient.h Miner.h picojson.h

clean :
	rm -f ${EXE} _kernel.h *.o libs/*.o _temp_*

re : clean all
