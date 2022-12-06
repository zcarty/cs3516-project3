/* Sets all vars for configuration*/
#include "include.h"
using namespace std;



struct fromConfig {
    int queueLength;
    int TTLVal;
    string ip_host;
    string ip_overlay;
    int sendDelay;
    int recieveDelay;
};

fromConfig config(int nodeID) {
    fromConfig out;
    
    // Deal with config.txt
    string buff;
    ifstream configFile;
    configFile.open("config.txt");
    
    // LINE 1
    int step;
    getline(configFile, buff);
    sscanf(buff.c_str(), "%d %d %d", &step, &out.queueLength, &out.TTLVal);

    // LINES 2-4
    int setupNode;
    char *ip_buff;
    for(int i = 0; i < 2; i++){
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
        sscanf(buff.c_str(), "%d %d", step, setupNode);
        if (setupNode[0] == nodeID) {
            out.ip_host = ip_buff;
            out.ip_overlay = ip_buff2;
        }
    }

    // work on delay (only need sendDelay (but for 2 other nodes))
    // // LINES 8-11
    // int sendNode;
    // int recNode;
    // int delayBuff;
    // int recBuff;
    // for(int i = 0; i < 3; i++){
    //     getline(configFile, buff);
    //     sscanf(buff.c_str(), "%d %d %d %d", step, sendNode, delayBuff, recNode, recBuff);
    //     if (sendNode == nodeID) {
    //         out.sendDelay
    //     }
    // }

    // LINES 11-13


    configFile.close();
    return out;
}