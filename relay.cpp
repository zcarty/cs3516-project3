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

    int socket = create_cs3516_socket();
    // int flags = fcntl(socket, F_GETFL, 0);
    // fcntl(socket, F_SETFL, (flags | O_NONBLOCK));

    // FOR TESTING PURPOSES
    if(strcmp(argv[1],"1") == 0) {
        printf("Sender designation made successfully.");
        cs3516_send(socket, "message", (sizeof("message")+1), h2);
    } else if(strcmp(argv[1],"2") == 0) {
        printf("Receiver designation made successfully.");
        int bytes_recvd = cs3516_recv(socket, SRC_BUFF, (sizeof(SRC_BUFF)-1));
        SRC_BUFF[bytes_recvd] = '\0'; printf("%s\n", SRC_BUFF);
    } else printf("Error: invalid designation");
}