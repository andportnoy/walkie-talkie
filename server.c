pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int recording;
void *keyboard_monitor(void *arg) {
	(void)arg;

	for (;;) {
		if (getchar() == '\n') {
			if (recording) {
				recording = 0;
				puts("done recording.");
				fflush(stdout);
			} else {
				pthread_mutex_lock(&lock);
				recording = 1;
				puts("Recording...");
				fflush(stdout);
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&lock);
			}
		}
	}
	return NULL;
}

void sendall(int sock, void *buf, size_t size) {
	char *ptr = buf;
	for (int rem=size, k; rem; rem-=k, ptr+=k) {
		k = send(sock, ptr, rem, 0);
		errif(k==-1, "send");
	}
}

double msdiff(void) {
	static int initialized = 0;
	static struct timespec prev, cur;
	if (!initialized) {
		clock_gettime(CLOCK_MONOTONIC, &cur);
		initialized = 1;
		return 0;
	}

	prev = cur;
	clock_gettime(CLOCK_MONOTONIC, &cur);
	return 1000*(cur.tv_sec-prev.tv_sec) + (cur.tv_nsec-prev.tv_nsec) / 1E6;
}

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

	patype *const buf = calloc(100*NFRAMES, sizeof *buf);
	for (;;) {
		struct sockaddr_storage csa = {0};
		socklen_t csalen = sizeof csa;
		int csock = accept(sock, (struct sockaddr *)&csa, &csalen);
		errif(csock==-1, "accept");
		printf("\taccept %s ->", addrstr((void *)&csa)); fflush(stdout);
		printf("\n");

		pthread_t mon;
		pthread_create(&mon, NULL, keyboard_monitor, NULL);

		patype *ptr = buf;

		pthread_mutex_lock(&lock);
		if (!recording)
			pthread_cond_wait(&cond, &lock);
		pthread_mutex_unlock(&lock);

		while (recording) {
			patype *data = audio_read();
			memcpy(ptr, data, NFRAMES*sizeof *data);
			ptr += NFRAMES;
		}

		sendall(csock, buf, (ptr-buf) * sizeof *ptr);

		errif(close(csock)==-1, "close csock");
		puts("close csock."); fflush(stdout);
	}
	errif(close(sock)==-1,  "close sock");
	puts("close sock.");

	audio_terminate();
	free(buf);
}
