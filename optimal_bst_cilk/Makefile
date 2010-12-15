CXX=icpc
#BITS=-m64
INCLUDES=-I.
CXXFLAGS=-O2 -tbb $(INCLUDES) $(BITS)

# If you are using a version of binutils prior to 2.17, 
# uncomment following line to work around a known issue. 
# please reference the Release Notes for more information. 
#LIBS=-lcilkrts

PROGS=optbst

all:: $(PROGS)

qsort: optbst.o
	$(CXX) $(BITS) -o $@ optbst.o $(LIBS)

clean::
	-rm -f $(PROGS) *.o
