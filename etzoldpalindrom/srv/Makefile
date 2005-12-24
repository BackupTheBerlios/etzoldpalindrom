TARGETS = epalsrv
CXX     = g++
INCS    = -I../lib -I..
LIBS    = -lgmp -lpthread -lssl

all: $(TARGETS)

epalsrv: srv.o ../lib/libpal.a
	$(CXX) $< ../lib/libpal.a $(LIBS) -o $@

epalsrv_static: srv.o ../lib/libpal.a
	$(CXX) -static $< ../lib/libpal.a $(LIBS) -o $@

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

