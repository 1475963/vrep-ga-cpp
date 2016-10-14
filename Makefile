##
## Aliases
##
CXX		= g++
CC		= gcc
RM		= rm -rf
MKDIR		= mkdir -p
SRCS_DIR	= ./srcs
VREP_DIR	= ./dependencies/remoteApi
BIN		= ./bin
BUILD		= ./build
PRGRM		= vrep-ga

##
## Good practices
##
CXXFLAGS	+= -W -Wextra -Wall -O3
CXXFLAGS	+= -fmax-errors=4

##
## Includes
##
CFLAGS		= -I$(VREP_DIR) -I./dependencies/includes/
CXXFLAGS	+= -I./includes/ -I$(VREP_DIR) -I./dependencies/includes/ -I./srcs/

##
## Libraries
##
LDFLAGS = -lpthread

##
## Version
##
CXXFLAGS += -std=c++11

##
## Variables
##
CFLAGS += -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255
CXXFLAGS += -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255

##
## OSes
##
OS = $(shell uname -s)
ifeq ($(OS), Linux)
    CFLAGS += -D__linux
    CXXFLAGS += -D__linux
else
    CFLAGS += -D__linux
    CXXFLAGS += -D__APPLE__
endif

##
## Sources and Objects
##
SRCS	= $(wildcard $(SRCS_DIR)/*.cpp)
OBJS	= $(patsubst $(SRCS_DIR)/%.cpp,$(BUILD)/%.o,$(SRCS))

##
## Compilation
##
all:			dir VREP $(BIN)/$(PRGRM)

dir:
			$(MKDIR) $(BIN) $(BUILD)

$(BIN)/$(PRGRM):	$(OBJS)
			$(CXX) $(CXXFLAGS) $(BUILD)/extApi.o $(BUILD)/extApiPlatform.o $(OBJS) -o $@ $(LDFLAGS)

VREP:
		$(CC) $(CFLAGS) -c $(VREP_DIR)/extApi.c -o $(BUILD)/extApi.o $(LDFLAGS)
		$(CC) $(CFLAGS) -c $(VREP_DIR)/extApiPlatform.c -o $(BUILD)/extApiPlatform.o $(LDFLAGS)

$(OBJS): $(BUILD)/%.o : $(SRCS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

##
## Clean
##
RM_OBJS = $(BUILD)

RM_BIN	= $(BIN)

clean:
	$(RM) $(RM_OBJS)

fclean: clean
	$(RM) $(RM_BIN)

re:	fclean all

##
## Avoid rules problems
##
.PHONY: all dir clean fclean re VREP
