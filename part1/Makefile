CXXFLAGS =  -g -Wall -D_FILE_OFFSET_BITS=64 -std=c++11
FUSEFLAGS= -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=25 -I/usr/local/include/fuse -I/usr/include/fuse
LDFLAGS = -L. -L/usr/lib
LDLIBS = -lfuse -pthread
RPCSTUFF = rpc.o chan.o host.o

all: rpctest lock_server lock_tester lock_demo

rpctest: rpctest.o ${RPCSTUFF}
	g++ ${LDLIBS} rpctest.o ${RPCSTUFF} -o rpctest

lock_demo: lock_demo.o lock_client.o ${RPCSTUFF}
	g++ ${LDLIBS} lock_demo.o lock_client.o ${RPCSTUFF} -o lock_demo

lock_tester: lock_tester.o lock_client.o ${RPCSTUFF}
	g++ ${LDLIBS} lock_tester.o lock_client.o ${RPCSTUFF} -o lock_tester

lock_server: lock_server.o lock_smain.o ${RPCSTUFF}
	g++ ${LDLIBS} lock_server.o lock_smain.o ${RPCSTUFF} -o lock_server

host.o: host.cc
	g++ -c ${CXXFLAGS} host.cc

chan.o: chan.cc
	g++ -c ${CXXFLAGS} chan.cc

rpc.o: rpc.cc chan.cc
	g++ -c ${CXXFLAGS} rpc.cc chan.cc

rpctest.o: rpctest.cc
	g++ -c ${CXXFLAGS} rpctest.cc

lock_demo.o: lock_demo.cc
	g++ -c ${CXXFLAGS} lock_demo.cc

lock_tester.o: lock_tester.cc
	g++ -c ${CXXFLAGS} lock_tester.cc

lock_client.o: lock_client.cc rpc.cc
	g++ -c ${CXXFLAGS} lock_client.cc rpc.cc

lock_server.o: lock_server.cc
	g++ -c ${CXXFLAGS} lock_server.cc

run-tester: lock_server lock_tester
	./lock_server 4000 > /dev/null 2>&1 &
	for i in $$(seq 5); do ./lock_tester 4000 $$i; done;
	kill -9 $$(ps -aux | grep 'lock_server' | awk '{print $$2}')


clean: 
	rm -rf *.o yfs_client extent_server lock_server lock_tester lock_demo rpctest 
