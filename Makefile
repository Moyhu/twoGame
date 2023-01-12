CROSS_PREFIX=
CFLAGS=-O3 -Werror -DPLATFORM_$(PLATFORM)=1

CXX=$(CROSS_PREFIX)g++
# target source
SRC  := $(wildcard *.cpp) 
deps := $(wildcard *.h) 
OBJ  := $(SRC:%.cpp=%.o)

TARGET := two
.PHONY : clean all

all: $(OBJ) $(TARGET) 

$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) -o $@ $^

$(OBJ):%.o:%.cpp $(deps)
	$(CXX) $(CFLAGS) -c $< -o $@ 

clean:
#	@rm -f $(TARGET)
	@rm -f $(OBJ)


