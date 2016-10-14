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
CFLAGS		+= -I$(VREP_DIR) -I./dependencies/includes/
CXXFLAGS	+= -I./includes/ -I$(VREP_DIR) -I./dependencies/includes

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

##
## OSes
##
OS = $(shell uname -s)
ifeq ($(OS), Linux)
    CXXFLAGS += -D__linux
else
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
all:			dir $(BIN)/$(PRGRM)

dir:			$(info $(SRCS)) $(info $(OBJS))
			$(MKDIR) $(BIN) $(BUILD)

$(BIN)/$(PRGRM):	$(OBJS)
			$(CC) $(CFLAGS) -c $(VREP_DIR)/extApi.c -o $(BUILD)/extApi.o
			$(CC) $(CFLAGS) -c $(VREP_DIR)/extApiPlatform.c -o $(BUILD)/extApiPlatform.o
			$(CXX) $(BUILD)/extApi.o $(BUILD)/extApiPlatform.o $(OBJS) -o $@ $(LDFLAGS)

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
.PHONY: all dir clean fclean re
