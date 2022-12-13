#include "include.h"
#include "cs3516sock.h"
#include "config.cpp"
using namespace std;

/*
 * relay.cpp
 * Zeb Carty and Michael McInerney
 *
 * Establishes behavior requirements and basis for communication between end-hosts and routers
 */

/** HELPER FUNCTIONS **/

/** MAIN FUNCTION **/
int main(int argc, char **argv)
{
    int nodeID = int(argv[1] - '0'); // convert nodeID to int
    fromConfig data = config(nodeID);

    int socket = 0;
    // defining buffers
    char SRC_BUFF[2001]; // additional byte to allow for space for null terminator
    char PAY_BUFF[1001]; // payload buffer, size can be adjusted later
    char ERR_BUFF[1001];

    unsigned magic_num = 0xFF10483C; // special packet designation indicating filesize

    // run the file with cmd args (nodeID)
    if (data.type == 1) // Router
    {
        // create socket s_addr change for multiple ips on one vm
        cout << "Creating socket...";
        socket = create_cs3516_socket(data.ip_host);
        cout << "done." << endl;

        char filename[] = "x_control.txt";
        filename[0] = nodeID + '0';
        ofstream logfile;
        struct timeval UNIX_TIME;

        while(1) {
            // First receive file size
            int recv_output = cs3516_recv(socket, SRC_BUFF, sizeof(SRC_BUFF));
            
            struct ip iphead;
            struct udphdr udphead;
            memcpy(&iphead,  SRC_BUFF,  sizeof(struct ip));
            memcpy(&udphead, SRC_BUFF + sizeof(struct ip),  sizeof(struct udphdr));
            memcpy(PAY_BUFF, SRC_BUFF + sizeof(struct ip) + sizeof(struct udphdr), (udphead.uh_ulen - sizeof(struct udphdr)));
            
            unsigned num_test = *PAY_BUFF;
            if((udphead.uh_ulen - sizeof(struct udphdr)) == 8 && num_test == magic_num) {
                int filesize = atoi((PAY_BUFF+4));
                int packet_num = ((filesize-1) / 1000) + 1;

                // TODO: Find destination and forward file-size packet to that destination
            } else {
                if(iphead.ip_ttl <= 1) {
                    gettimeofday(&UNIX_TIME, NULL);

                    logfile.open(filename);
                    logfile << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec/1000)   << "  "
                            << inet_ntoa(iphead.ip_src) << "  "
                            << inet_ntoa(iphead.ip_dst) << "  "
                            << iphead.ip_id << "  " << "TTL_EXPIRED" << endl;
                    logfile.close();
                    continue;
                } else {
                    iphead.ip_ttl--;
                    memcpy(SRC_BUFF,  &iphead,  sizeof(struct ip));

                    // TODO: Figure out what next hop is before enqueue, and drop packet if no route is resolved

                    if(enqueue(data, SRC_BUFF)) {
                        gettimeofday(&UNIX_TIME, NULL);

                        logfile.open(filename);
                        logfile << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec/1000)   << "  "
                            << inet_ntoa(iphead.ip_src) << "  "
                            << inet_ntoa(iphead.ip_dst) << "  "
                            << iphead.ip_id << "  " << "SENT_OKAY" << "  "
                            << "[NEXT HOP]" << endl;
                        logfile.close();
                    } else {
                        gettimeofday(&UNIX_TIME, NULL);

                        logfile.open(filename);
                        logfile << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec/1000)   << "  "
                            << inet_ntoa(iphead.ip_src) << "  "
                            << inet_ntoa(iphead.ip_dst) << "  "
                            << iphead.ip_id << "  " << "MAX_SENDQ_EXCEEDED" << endl;
                        logfile.close();
                    }
                }
            }

            // Forward packet to nextIP
           // string nextIP = searchTrie(&data.root, );
            for(int i; i < packet_num; i++)
            {
               // cs3516_send(socket, , , nextIP);
            }

        }
        /* router:
            call lookup function, which takes a destIP (overlay) and forwarding table returns new destIP(real)

            write to log file (ROUTER_control.txt)
            UNIXTIME SOURCE_OVERLAY_IP DEST_OVERLAY_IP IP_IDENT STATUS_CODE [NEXT_HOP]
    */
    }

    else // End Host
    {
        /* end host:
            when run:
            search for send_config.txt:
            destIP(overlay) sourcePort destPort
            search for send_body.txt:
            send file size, then
            divide the body into 1000 byte payloads and send to router

            when receiving:
            write stats to received_stats.txt
            write contents to file called received
            print to stdout size of transmitted file, # of packets transmitted
                        size of received file, # of packets received

            make a while loop to check contents of the endhost file (use select)
        */
    }


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

    // FOR TESTING PURPOSES
    if (strcmp(argv[1], "1") == 0)
    {
        struct ip iphead;
        char message[] = "message";
        iphead.ip_len = (20 + sizeof(struct udphdr) + sizeof(message)); // ip header + udp header + message
        iphead.ip_ttl = 3;                                              // time to live needs to change with increasing program sophistication
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
        memcpy((SRC_BUFF + sizeof(struct ip)), &udphead, sizeof(struct udphdr));
        memcpy((SRC_BUFF + sizeof(struct ip) + sizeof(struct udphdr)), message, sizeof(message));

        printf("Sender designation made successfully.\n");
        cout << "Sending packet...";
        cs3516_send(socket, SRC_BUFF, iphead.ip_len, h2);
        cout << "done." << endl;
    }
    else if (strcmp(argv[1], "2") == 0)
    {
        printf("Receiver designation made successfully.\n");
        printf("Listening for packet...");
        int bytes_recvd = cs3516_recv(socket, SRC_BUFF, (sizeof(SRC_BUFF) - 1));
        cout << "done." << endl;
        struct ip iphead;
        struct udphdr udphead;
        memcpy(&iphead, SRC_BUFF, sizeof(struct ip));
        memcpy(&udphead, SRC_BUFF + sizeof(struct ip), sizeof(struct udphdr));
        memcpy(PAY_BUFF, SRC_BUFF + sizeof(struct ip) + sizeof(struct udphdr), (udphead.uh_ulen - sizeof(struct udphdr)));

        // SRC_BUFF[bytes_recvd] = '\0';
        printf("%s\n", PAY_BUFF);
    }
    else
        printf("Error: invalid designation");
}