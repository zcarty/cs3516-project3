Zeb Carty and Michael McInerney

Creates an overlay network with following features:
 Create appropriate headers
 Packet forwarding, queuing, and TTL processing


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