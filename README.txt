Zeb Carty and Michael McInerney

Creates an overlay network with following features:
 Create appropriate headers
 Packet forwarding, queuing, and TTL processing

End-Hosts:
host1 (8114) 10.63.30.1
host2 (8115) 10.63.30.2
host3 (8116) 10.63.30.3

Routers:
host4 (8117) 10.63.30.4, 10.63.30.5, 10.63.30.6


To Use: (set up routers first on host4, then end-hosts)
run 'make clean'
run 'make relay'
run './relay <NODEID>' (making sure the config.txt and other required files are present in directory)

 todo:
 set up vms with files and ips
     // run the file with cmd args (1 for router, 2 for end host)
    /* end host:
    when run:
    search for send_config.txt:
    destIP(overlay) sourcePort destPort
    search for send_body.txt:
    divide the body into 1000 byte payloads and send to router

    when receiving:
    write stats to received_stats.txt
    write contents to file called received
    print to stdout size of transmitted file, # of packets transmitted
                    size of received file, # of packets received
    */

    /* router:
    call lookup function, which takes a destIP (overlay) and forwarding table returns new destIP(real)

    write to log file (ROUTER_control.txt)
    UNIXTIME SOURCE_OVERLAY_IP DEST_OVERLAY_IP IP_IDENT STATUS_CODE [NEXT_HOP]
    */