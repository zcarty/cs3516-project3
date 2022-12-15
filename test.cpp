#include "config.cpp"
#include "router.cpp"
#include "cs3516sock.cpp"
#include "include.h"
using namespace std;

int main(void)
{
    fromConfig data = config(1);

    std::string destIP = searchTrie(&data.root, "4.5.6.1");
    cout << "dest ip " << destIP << endl;
}