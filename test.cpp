#include "config.cpp"
#include "include.h"
using namespace std;

int main(void) {
    fromConfig data = config(1);
    cout << data.ip_host << endl;
    cout << data.ip_overlay << endl;
    cout << data.queueLength << endl;

    cout << data.delay[2] << endl;
    cout << data.delay[4] << endl;
}