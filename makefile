all: relay

relay: relay.cpp
	g++ relay.cpp -o relay

clean:
	rm -f relay