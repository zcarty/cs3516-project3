#include <string>
#include "router.h"

struct fromConfig
{
    int queueLength;
    int TTLVal;
    std::string ip_host;
    std::string ip_overlay;
    int delay[7];
    int type = 1;
    TrieNode root;
    std::string router_ip;
    int destID;
};

fromConfig config(int nodeID);

void processQueue(int socket, fromConfig data);

bool enqueue(fromConfig data, char *buffer, std::string ip_dest, int id_node);

