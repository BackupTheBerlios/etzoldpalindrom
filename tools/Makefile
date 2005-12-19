TARGETS = check pal
CXX     = g++
INCS    =
LIBS    = -lgmp -lpthread

all: $(TARGETS)

check: check.o libpal.o
	$(CXX) $< libpal.o $(LIBS) -o $@

pal: pal.o libpal.o number_generator.o
	$(CXX) $< libpal.o number_generator.o $(LIBS) -o $@

%.o: %.cc libpal.hh
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

