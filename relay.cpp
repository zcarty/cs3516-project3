#include "include.h"
#include "config.h"
#include "router.h"
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
int main(int argc, char **argv)
{
    int nodeID = int(argv[1][0] - '0'); // convert nodeID to int
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
        printf("RELAY INSTANCE DESIGNATED AS ROUTER\n");
        // create socket s_addr change for multiple ips on one vm
        std::cout << "Creating socket at ip: " << data.ip_host << "...";
        socket = create_cs3516_socket(data.ip_host);
        std::cout << "done." << std::endl;

        fd_set readfds;
        fd_set writefds;

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000 * data.delay[data.destID]; // Timeout based off of delay

        char filename[] = "x_control.txt";
        filename[0] = nodeID + '0';
        std::ofstream logfile;
        struct timeval UNIX_TIME;

        while (1)
        {
            // Iterate through queue
            processQueue(socket, data);

            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_SET(socket, &readfds);
            // Select only accepts if there is something to read
            int r = select(FD_SETSIZE, &readfds, &writefds, NULL, &tv);
            if (r <= 0)
                continue;

            // Receive filesize
            int recv_output = cs3516_recv(socket, SRC_BUFF, sizeof(SRC_BUFF));

            struct ip iphead;
            struct udphdr udphead;
            memcpy(&iphead, SRC_BUFF, sizeof(struct ip));
            memcpy(&udphead, SRC_BUFF + sizeof(struct ip), sizeof(struct udphdr));
            memcpy(PAY_BUFF, SRC_BUFF + sizeof(struct ip) + sizeof(struct udphdr), (udphead.uh_ulen - sizeof(struct udphdr)));

            unsigned num_test = *PAY_BUFF;
            int filesize;
            sscanf(PAY_BUFF, "%d", &filesize);
            cout << "filesize: " << filesize;
            int packet_num = ((filesize - 1) / 1000) + 1;

            // Find destination and forward file-size packet to that destination
            std::string destIP = searchTrie(&data.root, inet_ntoa(iphead.ip_dst));
            if (destIP == data.ip_host)
            { // send direct to end-host
                fromConfig destinationData = config(data.destID);
                printf("Sending file of %d bytes (%d packets) to %s...", filesize, packet_num, destinationData.ip_host.c_str());
                cs3516_send(socket, SRC_BUFF, recv_output, destinationData.ip_host);
                cout << "done." << endl;
            }
            else
            { // forward to another router
                printf("Sending file of %d bytes (%d packets) to %s. Routing packets to %s...", filesize, packet_num, inet_ntoa(iphead.ip_dst), destIP.c_str());
                cs3516_send(socket, SRC_BUFF, recv_output, destIP);
            }

            if (iphead.ip_ttl <= 1)
            {
                gettimeofday(&UNIX_TIME, NULL);

                logfile.open(filename);
                logfile << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec / 1000) << "  "
                        << inet_ntoa(iphead.ip_src) << "  "
                        << inet_ntoa(iphead.ip_dst) << "  "
                        << iphead.ip_id << "  "
                        << "TTL_EXPIRED" << std::endl;
                cout << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec / 1000) << "  "
                     << inet_ntoa(iphead.ip_src) << "  "
                     << inet_ntoa(iphead.ip_dst) << "  "
                     << iphead.ip_id << "  "
                     << "TTL_EXPIRED" << std::endl;
                logfile.close();
                continue;
            }
            else
            {
                iphead.ip_ttl--;
                // memcpy(SRC_BUFF, &iphead, sizeof(struct ip));

                // TODO: Figure out what next hop is before enqueue, and drop packet if no route is resolved
                std::string destIP = searchTrie(&data.root, inet_ntoa(iphead.ip_dst));
                int destNode = searchTrieForNode(&data.root, inet_ntoa(iphead.ip_dst));

                if (destIP == "IP not found")
                {
                    gettimeofday(&UNIX_TIME, NULL);

                    logfile.open(filename);
                    logfile << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec / 1000) << "  "
                            << inet_ntoa(iphead.ip_src) << "  "
                            << inet_ntoa(iphead.ip_dst) << "  "
                            << iphead.ip_id << "  "
                            << "NO_ROUTE_TO_HOST" << std::endl;

                    cout << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec / 1000) << "  "
                         << inet_ntoa(iphead.ip_src) << "  "
                         << inet_ntoa(iphead.ip_dst) << "  "
                         << iphead.ip_id << "  "
                         << "NO_ROUTE_TO_HOST" << std::endl;
                    logfile.close();
                    continue;
                }

                if (destIP == data.ip_host)
                {
                    fromConfig destinationData = config(data.destID);
                    destIP = destinationData.ip_host;
                    destNode = data.destID;
                }

                // QUEUE DROPPING
                if (enqueue(data, SRC_BUFF, destIP, destNode))
                {
                    gettimeofday(&UNIX_TIME, NULL);

                    logfile.open(filename);
                    logfile << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec / 1000) << "  "
                            << inet_ntoa(iphead.ip_src) << "  "
                            << inet_ntoa(iphead.ip_dst) << "  "
                            << iphead.ip_id << "  "
                            << "SENT_OKAY"
                            << "  "
                            << destIP << std::endl;
                    cout << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec / 1000) << "  "
                         << inet_ntoa(iphead.ip_src) << "  "
                         << inet_ntoa(iphead.ip_dst) << "  "
                         << iphead.ip_id << "  "
                         << "SENT_OKAY"
                         << "  "
                         << destIP << std::endl;
                    logfile.close();
                }
                else
                {
                    gettimeofday(&UNIX_TIME, NULL);

                    logfile.open(filename);
                    logfile << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec / 1000) << "  "
                            << inet_ntoa(iphead.ip_src) << "  "
                            << inet_ntoa(iphead.ip_dst) << "  "
                            << iphead.ip_id << "  "
                            << "MAX_SENDQ_EXCEEDED" << std::endl;
                    cout << UNIX_TIME.tv_sec << "." << (UNIX_TIME.tv_usec / 1000) << "  "
                         << inet_ntoa(iphead.ip_src) << "  "
                         << inet_ntoa(iphead.ip_dst) << "  "
                         << iphead.ip_id << "  "
                         << "MAX_SENDQ_EXCEEDED" << std::endl;
                    logfile.close();
                }
            }
        }
    }

    else // End Host
    {
        printf("RELAY INSTANCE DESIGNATED AS END HOST\n");
        std::cout << "Creating socket at ip: " << data.ip_host << "...";
        socket = create_cs3516_socket(data.ip_host);
        std::cout << "done." << std::endl;

        fd_set readfds;
        fd_set writefds;

        bool sent_file = false;

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 500000; // 0.5 sec timeout
        while (1)
        {
            // First check if incoming packets can be read
            /* Sets fds to zero */
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_SET(socket, &readfds);
            // Select only accepts if there is something to read
            int r = select(FD_SETSIZE, &readfds, &writefds, NULL, &tv);

            if (r > 0)
            {
                int bytes_recvd = cs3516_recv(socket, SRC_BUFF, (sizeof(SRC_BUFF) - 1));

                struct ip iphead;
                struct udphdr udphead;
                memcpy(&iphead, SRC_BUFF, sizeof(struct ip));
                memcpy(&udphead, SRC_BUFF + sizeof(struct ip), sizeof(struct udphdr));
                memcpy(PAY_BUFF, SRC_BUFF + sizeof(struct ip) + sizeof(struct udphdr), (udphead.uh_ulen - sizeof(struct udphdr)));

                std::string sourceIP = inet_ntoa(iphead.ip_src);
                int filesize;
                sscanf(PAY_BUFF, "%d", &filesize);
                int packet_num = ((filesize - 1) / 1000) + 1;

                printf("Receiving file of %d bytes (%d packets) from %s...", filesize, packet_num, sourceIP.c_str());

                std::ofstream received_stats;
                received_stats.open("received_stats.txt", std::ios_base::app);
                received_stats << "Source IP: " << inet_ntoa(iphead.ip_src) << "Dest IP: " << data.ip_overlay
                               << "Source Port: " << udphead.uh_sport << "Dest Port: " << udphead.uh_dport << std::endl;
                received_stats.close();

                std::ofstream received;
                received.open("received", std::ios_base::app);
                for (int i = 0; i < packet_num; i++)
                {
                    int bytes_recvd = cs3516_recv(socket, SRC_BUFF, (sizeof(SRC_BUFF) - 1));

                    if (bytes_recvd == -1)
                    {
                        cout << "Dropped packet from " << inet_ntoa(iphead.ip_src) << endl;
                    }

                    struct ip iphead;
                    struct udphdr udphead;
                    memcpy(&iphead, SRC_BUFF, sizeof(struct ip));
                    memcpy(&udphead, SRC_BUFF + sizeof(struct ip), sizeof(struct udphdr));
                    memcpy(PAY_BUFF, SRC_BUFF + sizeof(struct ip) + sizeof(struct udphdr), (udphead.uh_ulen - sizeof(struct udphdr)));
                    printf("%s\n", PAY_BUFF);
                    received << PAY_BUFF;
                }
                received << std::endl
                         << std::endl;
                received.close();
                std::cout << "done." << std::endl;
            }

            // Once packets are read, check for outbound packets
            // Once data from send_body has been sent, both send_config.txt and send_body.txt will be deleted
            std::string buff;
            std::fstream send_config;
            send_config.open("send_config.txt");
            if (send_config.peek() == std::ifstream::traits_type::eof())
            {
                continue;
            }

            int sourcePort;
            int destPort;
            getline(send_config, buff);
            std::string destIP = buff.substr(0, 7);
            sscanf(buff.substr(7).c_str(), "%d %d", &sourcePort, &destPort);

            send_config.close();
            send_config.open("send_config.txt", std::ofstream::out | std::ofstream::trunc);
            send_config.close();

            std::fstream send_body;
            send_body.open("send_body.txt");

            int begin = send_body.tellg();
            send_body.seekg(0, std::ios::end);
            int end = send_body.tellg();
            int filesize = end - begin;

            int packet_num = ((filesize - 1) / 1000) + 1;

            printf("Sending file of %d bytes (%d packets) to %s. Routing packets to %s...", filesize, packet_num, destIP.c_str(), data.router_ip.c_str());

            // Add headers
            struct ip iphead;
            char message[200];
            snprintf(message, sizeof(message), "%d", filesize);
            iphead.ip_len = (20 + sizeof(struct udphdr) + sizeof(message)); // ip header + udp header + message
            iphead.ip_ttl = data.TTLVal;
            inet_aton(data.ip_overlay.c_str(), &iphead.ip_src);
            inet_aton(destIP.c_str(), &iphead.ip_dst);
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
            cout << "filesize: " << message;
            cs3516_send(socket, SRC_BUFF, iphead.ip_len, data.router_ip);

            for (int i = 0; i < packet_num; i++)
            {
                int message_len = 1000;
                if (i == (packet_num - 1)) // Last packet
                {
                    message_len = filesize % 1000;
                }
                struct ip iphead;
                char message[message_len];
                send_body.read(message, message_len);

                iphead.ip_len = (20 + sizeof(struct udphdr) + sizeof(message)); // ip header + udp header + message
                iphead.ip_ttl = data.TTLVal;
                inet_aton(data.ip_overlay.c_str(), &iphead.ip_src);
                inet_aton(destIP.c_str(), &iphead.ip_dst);
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
                cs3516_send(socket, SRC_BUFF, iphead.ip_len, data.router_ip);
            }
            send_body.close();
            send_body.open("send_body.txt", std::ofstream::out | std::ofstream::trunc);
            send_body.close();
            std::cout << "done." << std::endl;
        }
    }
}