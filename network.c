void sendall(int sock, void *buf, size_t size) {
	char *ptr = buf;
	for (int rem=size, k; rem; rem-=k, ptr+=k) {
		k = send(sock, ptr, rem, 0);
		errif(k==-1, "send");
	}
}

/* returns socket listening on port */
int server(char *port) {
	int x;
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE,
	};
	struct addrinfo *res;

	x = getaddrinfo(NULL, port, &hints, &res);
	dieif(x, "getaddrinfo: %s", gai_strerror(x));

	int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	errif(sock==-1, "socket");

	x = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (int[]){1}, sizeof(int));
	errif(x==-1, "setsockopt");

	x = bind(sock, res->ai_addr, res->ai_addrlen);
	errif(x==-1, "bind");

	freeaddrinfo(res);

	x = listen(sock, BACKLOG);
	errif(x==-1, "listen");

	return sock;
}

/* returns socket connected to host on port */
int client(char *host, char *port) {
	int x;
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
	};
	struct addrinfo *res;
	x = getaddrinfo(host, port, &hints, &res);
	dieif(x, "getaddrinfo: %s", gai_strerror(x));

	int sock;
	struct addrinfo *p;
	for (p=res; p; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		errif(-1 == sock, "socket");

		x = connect(sock, p->ai_addr, p->ai_addrlen);
		errif(-1 == x, "connect");

		break;
	}

	dieif(!p, "failed to connect");

	return sock;
}

char *addrstr(struct sockaddr *sa) {
	char ip[INET6_ADDRSTRLEN];
	const void *addr;
	unsigned short port;
	if (sa->sa_family == AF_INET) {
		struct sockaddr_in *sai = (void *)sa;
		addr = &sai->sin_addr;
		port = ntohs(sai->sin_port);
	} else {
		struct sockaddr_in6 *sai6 = (void *)sa;
		addr = &sai6->sin6_addr;
		port = ntohs(sai6->sin6_port);
	}

	inet_ntop(sa->sa_family, addr, ip, sizeof ip);
	char buf[1024];
	sprintf(buf, "ip = %s, port = %d", ip, port);
	return strdup(buf);
}
