#source file search paths
VPATH = ../../src \
	../../../raven-set ../../../PathFinder/src ../../../quadtree/src

INCS = \
	-I../../../boost/boost1_72 \
	-I../../../raven-set \
	-I../../../PathFinder/src \
	-I../../../quadtree/src \
	-I../../src

# folder for .o files
ODIR=./obj

OPT=-O2

# sources
_TST = pickup.o cZone.o cOrder.o cRider.o \
	cPathFinder.o cGraph.o quadtree.o cRunWatch.o

TST = $(patsubst %,$(ODIR)/%,$(_TST))

$(ODIR)/%.o: %.cpp 
	g++ $(OPT)  -std=c++17  \
	-c -o $@ $< $(INCS)

test: $(TST)
	g++ $(OPT) \
	-o ../../bin/pickup.exe $^

