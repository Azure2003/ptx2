# File: Makefile
include $(FSLCONFDIR)/default.mk
NVCC = /usr/local/cuda/bin/nvcc
TARGET = tally

all: $(TARGET)

$(TARGET): main.o tally_gpu.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o tally_gpu.o -L/usr/local/cuda/lib64 -lcudart

main.o: main.cpp tally_gpu.h
	$(CXX) $(CXXFLAGS) -c main.cpp

tally_gpu.o: tally_gpu.cu tally_gpu.h
	$(NVCC) -c tally_gpu.cu -o tally_gpu.o

clean:
	rm -f *.o $(TARGET)
