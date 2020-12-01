int main(int argc, char **argv) {
	audio_initialize();

	int x;
	assert(argc == 2);
	char *host = argv[1];

	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
	};
	struct addrinfo *res;
	x = getaddrinfo(host, PORT, &hints, &res);
	dieif(x, "getaddrinfo: %s", gai_strerror(x));

	int sock;
	struct addrinfo *p;
	for (p=res; p; p = p->ai_next) {
		char *str = addrstr(p->ai_addr);
		printf("getaddrinfo: %s\n", str);
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (-1 == sock) {
			fprintf(stderr, "\tsocket: %s\n", strerror(errno));
			continue;
		} else {
			printf("\tsocket: success\n");
		}

		x = connect(sock, p->ai_addr, p->ai_addrlen);
		if (-1 == x) {
			fprintf(stderr, "\tconnect: %s\n", strerror(errno));
			continue;
		} else {
			printf("\tconnect: success\n");
		}
		struct sockaddr_storage ss = {0};
		socklen_t size = sizeof ss;
		x = getsockname(sock, (struct sockaddr *)&ss, &size);
		errif(-1==x, "getsockname");
		puts("\tgetsockname");
		printf("connected on %s\n", addrstr((struct sockaddr *)&ss));

		break;
	}

	dieif(!p, "failed to connect");

	patype buf[NFRAMES];
	for (;;) {
		char *ptr = (void *)buf;
		for (int rem = sizeof buf; rem; rem-=x, ptr+=x) {
			dieif(rem<0, "rem < 0");
			x = recv(sock, ptr, rem, 0);
			errif(x==-1, "\nrecv");
			if (x == 0)
				break;
		}
		if (x == 0)
			break;
		audio_write(buf);
	}

	close(sock);
	puts("close");

	audio_terminate();
}
