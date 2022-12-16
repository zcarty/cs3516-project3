Zeb Carty and Michael McInerney

Creates an overlay network with following features:
 Creates appropriate headers
 Packet forwarding using longest-prefix matching, drop-tail queueing, and TTL processing

Virtual Machine Info:
    Router:
    host1 (8114) 10.63.30.1 nodeID 1
    host2 (8115) 10.63.30.2 nodeID 2
    host3 (8116) 10.63.30.3 nodeID 3

    End-Hosts:
    host4 (8117) 10.63.30.4, 10.63.30.5, 10.63.30.6

To Use: (set up routers first on host4, then end-hosts) (we found that the VMs would sometimes delete the extra IPs on restart)
run 'sudo ip addr add 10.63.30.5/8 dev ens3'
run 'sudo ip addr add 10.63.30.6/8 dev ens3'
run 'make clean'
run 'make relay'
run './relay <NODEID>' (making sure the config.txt and other required files are present in directory)

A end-host will check for send_config and send_body every 0.5 seconds,
    if found, the host will send required packets, then clear both files
    to send a new file while the host is listening, write send_config and 
    send_body in another directory, then copy into host folder

Logs:
    For end-hosts, received packets will be logged in received_stats.txt and received
    NOTE: For routers, stats will be logged in <NODEID>_control.txt