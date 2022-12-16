/* Sets all vars for configuration*/
#include "include.h"
#include "cs3516sock.h"
#include "config.h"
#include "router.h"
using namespace std;

std::string *qdest = NULL;
char *queue = NULL;
int *qid = NULL;
int q_count = 0;
int q_start = 0;

struct timeval action_report[7];

fromConfig config(int nodeID)
{
    fromConfig out;

    // Deal with config.txt
    std::string buff;
    std::ifstream configFile;
    configFile.open("config.txt");

    string routers[3];

    // LINE 1
    int step;
    getline(configFile, buff);
    sscanf(buff.c_str(), "%d %d %d", &step, &out.queueLength, &out.TTLVal);

    // LINES 2-4
    int setupNode;
    char *ip_buff;
    for (int i = 0; i < 3; i++)
    {
        getline(configFile, buff);
        sscanf(buff.c_str(), "%d %d", &step, &setupNode);
        if (setupNode == nodeID)
        {
            out.ip_host = buff.substr(4);
        }
        routers[i] = buff.substr(4);
    }

    // LINES 5-7
    char *ip_buff2;
    for (int i = 0; i < 3; i++)
    {
        getline(configFile, buff);
        sscanf(buff.c_str(), "%d %d", &step, &setupNode);
        string real = buff.substr(4, 10);
        string overlay = buff.substr(15);
        insertNode(&out.root, overlay, routers[i], setupNode);
        if (setupNode == nodeID)
        {
            out.ip_host = real;
            out.ip_overlay = overlay;
        }
    }

    // LINES 8-11
    int sendNode;
    int sendDelay;
    int recNode;
    int recDelay;
    for (int i = 0; i < 3; i++)
    {
        getline(configFile, buff);
        sscanf(buff.c_str(), "%d %d %d %d %d", &step, &sendNode, &sendDelay, &recNode, &recDelay);
        if (sendNode == nodeID)
        {
            out.delay[recNode] = sendDelay;
        }
        else if (recNode == nodeID)
        {
            out.delay[sendNode] = recDelay;
        }
    }

    // LINES 11-13
    char *serverOverlay;
    char subnet[12];
    for (int i = 0; i < 3; i++)
    {
        getline(configFile, buff);
        sscanf(buff.c_str(), "%d %d %d %s %d %d", &step, &sendNode, &sendDelay, subnet, &recNode, &recDelay);
        // subnet[7] = '\0'; // set null terminator to void out actual subnet

        if (sendNode == nodeID)
        {
            out.delay[recNode] = sendDelay;
            out.destID = recNode;
        }
        else if (recNode == nodeID)
        {
            out.delay[sendNode] = recDelay;
        }
    }

    switch (nodeID)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        out.type = 2;
        out.router_ip = routers[0];
        break;
    case 5:
        out.type = 2;
        out.router_ip = routers[0];
        break;
    case 6:
        out.type = 2;
        out.router_ip = routers[0];
        break;

    default:
        break;
    }

    configFile.close();
    return out;
}

// bool enqueue(fromConfig data, char *buffer, std::string ip_dest, int id_node)
// {
//     // if (queue == NULL)
//     // {
//     //     // queue = (char *)malloc(data.queueLength * (2001));
//     //     // qid = (int *)malloc(data.queueLength * sizeof(int));
//     //     // qdest = (std::string *)malloc(data.queueLength * sizeof(std::string));
//     //     for (int i = 0; i < 7; i++)
//     //     {
//     //         action_report[i].tv_sec = -1;
//     //         action_report[i].tv_usec = -1;
//     //     }
//     // }
//     // if (q_count >= data.queueLength)
//     //     return false;
//     // else
//     // {
//     //     int index = (q_start + q_count) % data.queueLength;
//     //     memcpy((queue + 2001 * index), buffer, 2001);
//     //     qid[index] = id_node;
//     //     qdest[index] = ip_dest;
//     // } // figure out how to implement packet transmission delays, not necessarily but maybe here

//     return true;
// }

// void processQueue(int socket, fromConfig data)
// {
//     struct timeval t;
//     if (queue == NULL)
//         return;
//     for (int i = 0; i < q_count; i++)
//     {
//         int index = (q_start + i) % data.queueLength;
//         gettimeofday(&t, NULL);
//         int diff_secs = t.tv_sec - action_report[qid[index]].tv_sec;
//         int diff_usec = t.tv_usec - action_report[qid[index]].tv_usec;
//         diff_usec += diff_secs * 1e6;
//         if (action_report[qid[index]].tv_sec < 0 || diff_usec / 1000 >= data.delay[qid[index]])
//         {
//             struct udphdr info;
//             memcpy(&info, (queue + 2001 * index + sizeof(struct ip)), sizeof(struct udphdr));
//             cs3516_send(socket, (queue + 2001 * index), (info.uh_ulen + sizeof(struct ip)), qdest[index]);
//             action_report[qid[index]].tv_sec = t.tv_sec;
//             action_report[qid[index]].tv_usec = t.tv_usec;

//             for (int j = i - 1; j < q_count; j++)
//             {
//                 int index2 = (q_start + j) % data.queueLength;
//                 index = (q_start + j - 1) % data.queueLength;

//                 memcpy((queue + 2001 * index), (queue + 2001 * index2), 2001);
//                 qid[index] = qid[index2];
//                 qdest[index] = qdest[index2];
//             }
//             q_count--;
//             processQueue(socket, data);
//             return;
//         }
//     }
// }