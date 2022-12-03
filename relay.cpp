#include "include.h"
#include "cs3516sock.h"
using namespace std;

/*
 * relay.cpp
 * Zeb Carty and Michael McInerney
 *
 * Establishes behavior requirements and basis for communication between end-hosts and routers
 */

/** HELPER FUNCTIONS **/


/** MAIN FUNCTION **/
int main(int argc, char** argv) {
    // defining buffers
    char SRC_BUFF[1001]; // additional byte to allow for space for null terminator
    char PAY_BUFF[1001]; // payload buffer, size can be adjusted later
    char ERR_BUFF[1001];
    // predefine host and router ips, this should change in the future if my assumptions are correct
    struct in_addr adr1;
    struct in_addr adr2;
    const char *ip_host1 = "10.63.20.1";
    const char *ip_host2 = "10.63.20.2";

    inet_aton(ip_host1, &adr1);
    inet_aton(ip_host2, &adr2);

    // be sure to implement error checking for ips in future program iterations
    unsigned long h1 = adr1.s_addr;
    unsigned long h2 = adr2.s_addr;

    cout << "Creating socket...";
    int socket = create_cs3516_socket();
    cout << "done." << endl;
    // int flags = fcntl(socket, F_GETFL, 0);
    // fcntl(socket, F_SETFL, (flags | O_NONBLOCK));




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
    
    // Creates Trie structure from config file (do in relay.cpp)
    call lookup function, which takes a destIP (overlay) and forwarding table returns new destIP(real)

    write to log file (ROUTER_control.txt)
    UNIXTIME SOURCE_OVERLAY_IP DEST_OVERLAY_IP IP_IDENT STATUS_CODE [NEXT_HOP]
    */



    // FOR TESTING PURPOSES
    if(strcmp(argv[1],"1") == 0) {
        struct ip iphead;
        char message[] = "message";
        iphead.ip_len = (20+sizeof(struct udphdr)+sizeof(message)); // ip header + udp header + message
        iphead.ip_ttl = 3; // time to live needs to change with increasing program sophistication
        iphead.ip_src = adr1;
        iphead.ip_dst = adr2;
        iphead.ip_tos = 0;
        iphead.ip_off = 0;
        iphead.ip_sum = 0;
        iphead.ip_hl = 5;
        iphead.ip_id = 0; // ip_id will increment as packets are transmitted
        iphead.ip_p = 17; // relevant to udp
        iphead.ip_v = 4;

        struct udphdr udphead;
        udphead.uh_ulen = (sizeof(struct udphdr) + sizeof(message));
        udphead.uh_sport = MYPORT;
        udphead.uh_dport = MYPORT;
        udphead.uh_sum = 0;

        memcpy(SRC_BUFF, &iphead, sizeof(struct ip));
        memcpy((SRC_BUFF+sizeof(struct ip)), &udphead, sizeof(struct udphdr));
        memcpy((SRC_BUFF+sizeof(struct ip)+sizeof(struct udphdr)), message, sizeof(message));

        printf("Sender designation made successfully.\n");
        cout << "Sending packet...";
        cs3516_send(socket, SRC_BUFF, iphead.ip_len, h2);
        cout << "done." << endl;

    } else if(strcmp(argv[1],"2") == 0) {
        printf("Receiver designation made successfully.\n");
        printf("Listening for packet...");
        int bytes_recvd = cs3516_recv(socket, SRC_BUFF, (sizeof(SRC_BUFF)-1));
        cout << "done." << endl;
        struct ip iphead;
        struct udphdr udphead;
        memcpy(&iphead, SRC_BUFF, sizeof(struct ip));
        memcpy(&udphead, SRC_BUFF+sizeof(struct ip), sizeof(struct udphdr));
        memcpy(PAY_BUFF, SRC_BUFF+sizeof(struct ip)+sizeof(struct udphdr), (udphead.uh_ulen-sizeof(struct udphdr)));

        //SRC_BUFF[bytes_recvd] = '\0'; 
        printf("%s\n", PAY_BUFF);
    } else printf("Error: invalid designation");

}