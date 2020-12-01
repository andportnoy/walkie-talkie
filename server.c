int main(void) {
	audio_initialize();

	int x; /* status/return value */
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE,
	};
	struct addrinfo *res;

	x = getaddrinfo(NULL, PORT, &hints, &res);
	dieif(x, "getaddrinfo: %s", gai_strerror(x));
	printf("getaddrinfo: %s\n", addrstr(res->ai_addr));

	int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	errif(sock==-1, "socket");
	printf("socket ->");

	x = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (int[]){1}, sizeof(int));
	errif(x==-1, "setsockopt");
	printf(" setsockopt ->");

	x = bind(sock, res->ai_addr, res->ai_addrlen);
	errif(x==-1, "bind");
	printf(" bind ->");

	freeaddrinfo(res);
	printf(" freeaddrinfo ->");

	x = listen(sock, BACKLOG);
	errif(x==-1, "listen");
	printf(" listen ->\n");

	for (;;) {
		struct sockaddr_storage csa = {0};
		socklen_t csalen = sizeof csa;
		int csock = accept(sock, (struct sockaddr *)&csa, &csalen);
		errif(csock==-1, "accept");
		printf("\taccept %s ->", addrstr((void *)&csa)); fflush(stdout);
		printf("\n");

		for (;;) {
			patype *data = audio_read();
			char *ptr = (void *)data;
			for (int rem=NFRAMES*sizeof *data; rem; rem-=x,ptr+=x) {
				x = send(csock, ptr, rem, 0);
				errif(x==-1, "send");
			}
		}
		errif(close(csock)==-1, "close csock");
		printf(" close csock.\n"); fflush(stdout);
	}
	errif(close(sock)==-1,  "close sock");
	puts("close sock.");

	audio_terminate();
}
