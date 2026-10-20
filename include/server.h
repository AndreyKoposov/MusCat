#include <netinet/in.h>


int send_all(int fd, char* buf, ssize_t bytes);

void try_close(int fd);

int start(int port);