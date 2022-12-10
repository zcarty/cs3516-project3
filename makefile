all: relay

relay: relay.cpp router.cpp config.cpp
	g++ relay.cpp -o relay

clean:
	rm -f relay test a.out