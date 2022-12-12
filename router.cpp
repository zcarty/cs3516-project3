/* Contains trie data structure for router */

#include "include.h"
using namespace std;
 

struct TrieNode
{
    struct TrieNode *children[2];
    string nextHop = NULL;
};
 
// Returns node in Trie
struct TrieNode *getNode(void)
{
    struct TrieNode *newNode =  new TrieNode;
 
    for (int i = 0; i < 2; i++)
        newNode->children[i] = NULL;
 
    return newNode;
}
 
// Takes overlay IP and real IP and adds them to trie
void insertNode(struct TrieNode *root, string overlayIP, string realIP)
{ 
    for (int i = 0; i < overlayIP.length(); i++)
    {
        int index = int(overlayIP[i] - '0');
        if (root->children[index] == NULL) {
            root->children[index] = getNode();
            break;
        }
        root = root->children[index];
    }
 
    root->nextHop = realIP;
}
// Searches Trie for overlayIP and returns best destIP
string searchTrie(struct TrieNode *root, string overlayIP)
{
    for (int i = 0; i < overlayIP.length(); i++)
    {
        int index = int(overlayIP[i] - '0');
        if (root->children[index] != NULL) {
            return root->children[index]->nextHop;
        }
        root = root->children[index];
    }
    return "IP not found";
}