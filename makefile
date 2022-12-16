all: relay

relay: relay.cpp router.cpp config.cpp cs3516sock.cpp
	g++ relay.cpp router.cpp config.cpp cs3516sock.cpp -o relay

clean:
	rm -f relay test a.out

remove:
	rm -f received received_stats.txt