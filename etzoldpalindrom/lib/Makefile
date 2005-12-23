TARGETS = libpal.a
CXX     = g++
INCS    = -I..
OBJS    = libpal.o number_generator.o args.o socket.o logging.o sha2.o diffie_hellman.o

all: $(TARGETS)

libpal.a: $(OBJS)
	ar crs $@ $(OBJS)

%.o: %.cc %.hh
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

