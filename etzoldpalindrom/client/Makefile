TARGETS = client
CXX     = g++
INCS    = -I../lib -I..
LIBS    = /usr/lib/libgmp.a -lpthread

all: $(TARGETS)

client: client.o ../lib/libpal.a
	$(CXX) $< ../lib/libpal.a $(LIBS) -o $@
	$(CXX) -static $< ../lib/libpal.a $(LIBS) -o client_static

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

