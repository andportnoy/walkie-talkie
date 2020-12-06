#define BACKLOG 10

int server(char *port);
int client(char *host, char *port);
ssize_t sendall(int sock, void *buf, ssize_t size);
ssize_t recvall(int sock, void *buf, ssize_t size);
char *addrstr(struct sockaddr *sa);
