#include <string>

#pragma once

struct TrieNode
{
    struct TrieNode *children[10] = {NULL};
    std::string nextHop;
    int nodeID;
};

struct TrieNode *getNode(void);


void insertNode(struct TrieNode *root, std::string overlayIP, std::string realIP, int nodeID);

std::string searchTrie(struct TrieNode *root, std::string overlayIP);

int searchTrieForNode(struct TrieNode *root, std::string overlayIP);
