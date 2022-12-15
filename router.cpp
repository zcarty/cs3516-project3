/* Contains trie data structure for router */

#include "include.h"
#include "router.h"
//using namespace std;
 


 
// Returns node in Trie
struct TrieNode *getNode(void)
{
    struct TrieNode *newNode =  new TrieNode;
 
    for (int i = 0; i < 2; i++)
        newNode->children[i] = NULL;
 
    return newNode;
}
 
// Takes overlay IP and real IP and adds them to trie
void insertNode(struct TrieNode *root, std::string overlayIP, std::string realIP, int nodeID)
{
    in_addr_t bin_overlay = inet_addr(overlayIP.c_str()); // convert overlayIP to binary format

    for (int i = 31; i >= 8; i--) 
    {
        int index = (bin_overlay >> i) & 0x1;
        if (root->children[index] == NULL) {
            root->children[index] = getNode();
        }
        root = root->children[index];
    }

    root->nextHop = realIP;
    root->nodeID = nodeID;
}
// Searches Trie for overlayIP and returns best destIP
std::string searchTrie(struct TrieNode *root, std::string overlayIP)
{
    in_addr_t bin_overlay = inet_addr(overlayIP.c_str()); // convert overlayIP to binary format

    for (int i = 31; i >= 0; i--)
    {
        int index = (bin_overlay >> i) & 0x1;
        if (root->children[index] == NULL) {
            return root->nextHop;
        }
        root = root->children[index];
    }
    return "IP not found";
}

// Override for searchTrie above that includes nodeID
int searchTrieForNode(struct TrieNode *root, std::string overlayIP)
{
    in_addr_t bin_overlay = inet_addr(overlayIP.c_str()); // convert overlayIP to binary format

    for (int i = 31; i >= 0; i--)
    {
        int index = (bin_overlay >> i) & 0x1;
        if (root->children[index] == NULL) {
            return root->nodeID;
        }
        root = root->children[index];
    }
    return -1;
}