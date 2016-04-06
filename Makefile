all: bpw.x

CC=g++
OPT= -g -std=c++0x
MSRDEVNAME=msr
LIB= -lpthread -lrt

bpw.x: bpw.cpp ./intel/cpucounters.cpp ./intel/msr.cpp ./intel/pci.cpp ./intel/client_bw.cpp
	$(CC) $(OPT) -O bpw.cpp ./intel/cpucounters.cpp ./intel/msr.cpp ./intel/pci.cpp ./intel/client_bw.cpp -o bpw -I ./intel $(LIB)
