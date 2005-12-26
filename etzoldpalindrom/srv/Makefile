TARGETS     = epalsrv epalsrv_static
CXX         = g++
INCS        = -I../lib -I..
LIBS        = -lgmp -lpthread -lssl -lgdbm
LIBS_STATIC = -lcrypto -lgssapi_krb5 -lkrb5 -lk5crypto -lkrb5support -lcom_err -lresolv
OBJS        = srv.o accounts.o

all: $(TARGETS)

epalsrv: $(OBJS) ../lib/libpal.a
	$(CXX) $(OBJS) ../lib/libpal.a $(LIBS) -o $@

epalsrv_static: $(OBJS) ../lib/libpal.a
	$(CXX) -static $(OBJS) ../lib/libpal.a $(LIBS) $(LIBS_STATIC) -o $@

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

