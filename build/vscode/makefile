#source file search paths
VPATH = ../../src ../../src/ext \
	../../../raven-set ../../../PathFinder/src ../../../quadtree/src

INCS = \
	-I../../../boost/boost1_72 \
	-I../../../raven-set \
	-I../../../PathFinder/src \
	-I../../../quadtree/src \
	-I../../../windex/include \
	-I../../src

LIBS = -lstdc++fs -lgdi32 -lcomdlg32 -lws2_32 -lwsock32

# folder for .o files
ODIR=./obj

OPT=-O2

# sources
_TST = pickup.o cZone.o cOrder.o cRider.o \
	cPathFinder.o cGraph.o quadtree.o cRunWatch.o \
	raven_sqlite.o sqlite3.o

_SERVER = server.o cZone.o cOrder.o cRider.o config.o \
	cPathFinder.o cGraph.o quadtree.o cRunWatch.o \
	raven_sqlite.o sqlite3.o

_GUI = gui.o config.o \
	raven_sqlite.o sqlite3.o


TST = $(patsubst %,$(ODIR)/%,$(_TST))
SERVER = $(patsubst %,$(ODIR)/%,$(_SERVER))
GUI = $(patsubst %,$(ODIR)/%,$(_GUI))

# compile

$(ODIR)/sqlite3.o: sqlite3.c
	gcc	 -c -o $@ $<
$(ODIR)/%.o: %.cpp 
	g++ $(OPT)  -std=c++17 -D_mingw_  \
	-c -o $@ $< $(INCS)

# link

test: $(TST)
	g++ $(OPT) \
	-o ../../bin/pickup.exe \
	$^ $(LIBS)

server: $(SERVER)
	g++ $(OPT) \
	-o ../../bin/server.exe \
	$^ $(LIBS)

gui: $(GUI)
	g++ $(OPT) \
	-o ../../bin/gui.exe \
	$^ $(LIBS)

demo: $(DEMO)
	g++ $(OPT) \
	-o ../../bin/demo.exe \
	$^ $(LIBS)

.PHONY: clean
clean:
	del /q .\obj