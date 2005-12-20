TARGETS = libpal.a
CXX     = g++
INCS    =
OBJS    = libpal.o number_generator.o args.o

all: $(TARGETS)

libpal.a: $(OBJS)
	@ar crs $@ *.o

%.o: %.cc %.hh
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

