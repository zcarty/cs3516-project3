#include "config.cpp"
#include "include.h"
using namespace std;

int main(void) {
    fromConfig data = config(1);
    cout << data.ip_host << endl;
    cout << data.queueLength << endl;
}