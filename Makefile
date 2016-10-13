CFLAGS = -I./remoteApi 			              \
				 -I./include 				              \
				 -std=c++11               \
				 -DNON_MATLAB_PARSING             \
				 -DMAX_EXT_API_CONNECTIONS=255

OS = $(shell uname -s)
ifeq ($(OS), Linux)
    CFLAGS += -D__linux
else
    CFLAGS += -D__APPLE__
endif

all:
	@rm -f bin/*.*
	@rm -f *.o
	g++ $(CFLAGS) -c ./srcs/test.cpp -o test.o
	g++ $(CFLAGS) -c ./srcs/VrepConnection.cpp -o VrepConnection.o
	g++ $(CFLAGS) -c ./remoteApi/extApi.c -o extApi.o
	g++ $(CFLAGS) -c ./remoteApi/extApiPlatform.c -o extApiPlatform.o
	@mkdir -p bin
	g++ extApi.o extApiPlatform.o test.o VrepConnection.o -o bin/test -lpthread
