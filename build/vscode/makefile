#source file search paths
VPATH = ../../src ../../../raven-set

INCS = \
	-I../../../boost/boost1_72 \
	-I../../../raven-set \
	-I../../src

# folder for .o files
ODIR=./obj

# sources
_TST = pickup.o  cRunWatch.o
TST = $(patsubst %,$(ODIR)/%,$(_TST))

$(ODIR)/%.o: %.cpp 
	g++ -O2  -std=c++17  \
	-c -o $@ $< $(INCS)

test: $(TST)
	g++ -O2 \
	-o ../../bin/pickup.exe $^

