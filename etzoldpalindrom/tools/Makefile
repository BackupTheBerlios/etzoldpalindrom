TARGETS = check pal
CXX     = g++
INCS    = -I../lib
LIBS    = -lgmp -lpthread

all: $(TARGETS)

check: check.o ../lib/libpal.a
	$(CXX) $< ../lib/libpal.a $(LIBS) -o $@

pal: pal.o ../lib/libpal.a
	$(CXX) $< ../lib/libpal.a $(LIBS) -o $@

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

