TARGETS = test_diffie_hellman
CXX     = g++
INCS    = -I../lib
LIBS    = -lgmp -lpthread

all: $(TARGETS)

test_diffie_hellman: test_diffie_hellman.o ../lib/libpal.a
	$(CXX) $< ../lib/libpal.a $(LIBS) -o $@

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

