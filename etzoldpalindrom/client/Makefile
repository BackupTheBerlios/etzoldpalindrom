TARGETS = epal
CXX     = g++
INCS    = -I../lib -I..
LIBS    = -lgmp -lpthread -lssl

all: $(TARGETS)

epal: epal.o ../lib/libpal.a
	$(CXX) $< ../lib/libpal.a $(LIBS) -o $@

epal_static: epal.o ../lib/libpal.a
	$(CXX) -static $< ../lib/libpal.a $(LIBS) -o $@

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

