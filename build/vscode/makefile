#source file search paths
VPATH = ../../src ../../../raven-set ../../../PathFinder/src

INCS = \
	-I../../../boost/boost1_72 \
	-I../../../raven-set \
	-I../../../PathFinder/src \
	-I../../src

# folder for .o files
ODIR=./obj

# sources
_TST = pickup.o cStack.o \
	cPathFinder.o cGraph.o cRunWatch.o
	
TST = $(patsubst %,$(ODIR)/%,$(_TST))

$(ODIR)/%.o: %.cpp 
	g++ -O2  -std=c++17  \
	-c -o $@ $< $(INCS)

test: $(TST)
	g++ -O2 \
	-o ../../bin/pickup.exe $^

