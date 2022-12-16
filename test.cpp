// #include "config.cpp"
// #include "router.cpp"
// #include "cs3516sock.cpp"
#include "include.h"
using namespace std;

int main(void)
{
    char PAY_BUFF[] = {"HELLO"};


    char rec_message[1000];
    sscanf(PAY_BUFF, "%s", &rec_message);

    cout << "Receieved message: " << rec_message;

}