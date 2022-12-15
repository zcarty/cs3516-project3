#include <string>

#define MYPORT 5950


int create_cs3516_socket(std::string address);
int cs3516_recv(int sock, char *buffer, int buff_size);
int cs3516_send(int sock, char *buffer, int buff_size, std::string nextIP);

