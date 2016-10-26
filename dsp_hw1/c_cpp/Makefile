# .PHONY: all clean

CXXFLAGS+= -O2 -Wall
LDFLAGS+=-lm     # link to math library

TARGET=train test acc

all: $(TARGET)
# type make/make all to compile test_hmm

train: train.cpp
	g++ train.cpp $(CXXFLAGS) -o train

test: test.cpp
	g++ test.cpp $(CXXFLAGS) -o test

acc: acc.cpp
	g++ acc.cpp $(CXXFLAGS) -o acc

clean:
	$(RM) $(TARGET)   # type make clean to remove the compiled file

