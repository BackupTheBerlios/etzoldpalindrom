TARGETS     = epal
CXX         = g++
INCS        = -I../lib -I..
LIBS        = -lgmp -lpthread -lssl
STATIC_LIBS = -lcrypto -lgssapi_krb5 -lkrb5 -lk5crypto -lkrb5support -lcom_err -lresolv
OBJS        = epal.o srv.o usr.o

all: $(TARGETS)

epal: $(OBJS) ../lib/libpal.a
	$(CXX) $(OBJS) ../lib/libpal.a $(LIBS) -o $@

epal_static: $(OBJS) ../lib/libpal.a
	$(CXX) -static $(OBJS) ../lib/libpal.a $(STATIC_LIBS) -o $@

%.o: %.cc
	$(CXX) -c -O2 $(INCS) $<

clean:
	rm -f *.o $(TARGETS)

