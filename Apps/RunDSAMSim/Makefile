# Makefile for Octave extension RunDSAMSim

OBJS	= DSAMMatrix.o
INSTALLDIR=.
PROGRAM = RunDSAMSim.oct
#PROGRAM = RunDSAMSim.mexglx
DEBUG	= -ggdb3

.phony: all

all: $(PROGRAM)

.phony: clean

clean:
	rm -f $(PROGRAM) *.o

.phony: distclean

distclean: clean
	rm -f *~ octave-core core

.phony: test

test: RunDSAMSim.oct
	octave -silent testmp.m

RunDSAMSim.oct:  RunDSAMSimOct.cpp $(OBJS)
	CPPFLAGS=$(DEBUG) mkoctfile -o $@ RunDSAMSimOct.cpp $(OBJS) \
	  `dsam-config --cflags --libs`

RunDSAMSim.mexglx:  RunDSAMSimMat.c $(OBJS)
	CPPFLAGS=$(DEBUG) mex -o $@ RunDSAMSimMat.c $(OBJS) \
	  `dsam-config --cflags --libs`

DSAMMatrix.o: DSAMMatrix.c DSAMMatrix.h
	$(CC) -c $< $(DEBUG) `dsam-config --cflags`
