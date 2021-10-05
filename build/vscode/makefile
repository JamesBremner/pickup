#source file search paths
VPATH = ../../src ../../src/ext \
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
	cPathFinder.o cGraph.o quadtree.o cRunWatch.o \
	raven_sqlite.o sqlite3.o

TST = $(patsubst %,$(ODIR)/%,$(_TST))

$(ODIR)/sqlite3.o: sqlite3.c
	gcc	 -c -o $@ $<
$(ODIR)/%.o: %.cpp 
	g++ $(OPT)  -std=c++17 -D_mingw_  \
	-c -o $@ $< $(INCS)

test: $(TST)
	g++ $(OPT) \
	-o ../../bin/pickup.exe \
	$^ \
	-lstdc++fs


.PHONY: clean
clean:
	del /q .\obj