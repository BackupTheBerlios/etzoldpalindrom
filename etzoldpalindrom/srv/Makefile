TARGETS = main
CXX     = g++
INCS    = -I../lib
LIBS    = -lgmp -lpthread

all: $(TARGETS)

main: main.o socket.o ../lib/libpal.a
	$(CXX) $< socket.o ../lib/libpal.a $(LIBS) -o $@

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

