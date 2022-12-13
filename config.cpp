/* Sets all vars for configuration*/
#include "include.h"
#include "router.cpp"
using namespace std;

char* queue = NULL;
int q_count = 0;
int q_start = 0;

struct fromConfig {
    int queueLength;
    int TTLVal;
    string ip_host;
    string ip_overlay;
    int delay[7];
    int type = 1;
    TrieNode root;
};

fromConfig config(int nodeID) {
    fromConfig out;
    
    // Deal with config.txt
    string buff;
    ifstream configFile;
    configFile.open("config.txt");
    
    if (nodeID == 4 || nodeID == 5 || nodeID == 6)
    {
        out.type = 2; // End-Host
    }

    // LINE 1
    int step;
    getline(configFile, buff);
    sscanf(buff.c_str(), "%d %d %d", &step, &out.queueLength, &out.TTLVal);

    // LINES 2-4
    int setupNode;
    char *ip_buff;
    for(int i = 0; i < 3; i++){
        getline(configFile, buff);
        sscanf(buff.c_str(), "%d %d", &step, &setupNode);
        if (setupNode == nodeID) {
            out.ip_host = buff.substr(4);
        }
    }

    // LINES 5-7
    char *ip_buff2;
    for(int i = 0; i < 3; i++){
        getline(configFile, buff);
        sscanf(buff.c_str(), "%d %d", &step, &setupNode);
        if (setupNode == nodeID) {
            out.ip_host = buff.substr(4,10);
            out.ip_overlay = buff.substr(15);
        }
        // Creates Trie Structure
        else if (out.type == 1) {
            insertNode(&out.root, buff.substr(15), buff.substr(4,10));
        }
    }

    // LINES 8-11
    int sendNode;
    int sendDelay;
    int recNode;
    int recDelay;
    for(int i = 0; i < 3; i++){
        getline(configFile, buff);
        sscanf(buff.c_str(), "%d %d %d %d %d", &step, &sendNode, &sendDelay, &recNode, &recDelay);
        if (sendNode == nodeID) {
            out.delay[recNode] = sendDelay;
        }
        else if (recNode == nodeID) {
            out.delay[sendNode] = recDelay;
        }
    }

    // LINES 11-13
    char *serverOverlay;
    for(int i = 0; i < 3; i++){
        getline(configFile, buff);
        sscanf(buff.c_str(), "%d %d %d", &step, &sendNode, &sendDelay);
        sscanf(buff.substr(20).c_str(), "%d %d", &recNode, &recDelay);
        if (sendNode == nodeID) {
            out.delay[recNode] = sendDelay;
        }
        else if (recNode == nodeID) {
            out.delay[sendNode] = recDelay;
        }
    }

    configFile.close();
    return out;
}

bool enqueue(fromConfig data, char* buffer)
{
    if(queue == NULL) queue = (char*) malloc(data.queueLength*(2001));
    if(q_count >= data.queueLength) return false;
    else {
        int index = (q_start + q_count) % data.queueLength;
        memcpy((queue+2001*index), buffer, 2001);
    } // figure out how to implement packet transmission delays, not necessarily but maybe here

    return true;
}