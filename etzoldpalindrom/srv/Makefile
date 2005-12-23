TARGETS = epalsrv epalsrv_static
CXX     = g++
INCS    = -I../lib -I..
LIBS    = /usr/lib/libgmp.a -lpthread

all: $(TARGETS)

epalsrv: srv.o ../lib/libpal.a
	$(CXX) $< ../lib/libpal.a $(LIBS) -o $@

epalsrv_static: srv.o ../lib/libpal.a
	$(CXX) -static $< ../lib/libpal.a $(LIBS) -o $@

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

