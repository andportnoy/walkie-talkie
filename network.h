#define BACKLOG 10

int server(char *port);
int client(char *host, char *port);
int sendall(int sock, void *buf, size_t size);
int recvall(int sock, void *buf, size_t size);
char *addrstr(struct sockaddr *sa);
