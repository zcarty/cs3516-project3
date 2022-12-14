#include "config.cpp"
#include "include.h"
using namespace std;

int main(void)
{
    struct ip iphead;
    struct udphdr udphead;
    cout << "ip " << sizeof(struct ip) << " udp " << sizeof(struct udphdr);
}