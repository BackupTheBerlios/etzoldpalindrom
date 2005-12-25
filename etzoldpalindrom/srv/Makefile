TARGETS = epalsrv
CXX     = g++
INCS    = -I../lib -I..
LIBS    = -lgmp -lpthread -lssl -lgdbm
OBJS    = srv.o accounts.o

all: $(TARGETS)

epalsrv: $(OBJS) ../lib/libpal.a
	$(CXX) $(OBJS) ../lib/libpal.a $(LIBS) -o $@

epalsrv_static: $(OBJS) ../lib/libpal.a
	$(CXX) -static $(OBJS) ../lib/libpal.a $(LIBS) -o $@

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

