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

/* returns socket listening on port */
int serve(void) {
	int x;
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE,
	};
	struct addrinfo *res;

	x = getaddrinfo(NULL, PORT, &hints, &res);
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

int main(void) {
	audio_initialize();

	int sock = serve();
	log("Listening for connections...");
	for (;;) {
		int csock = accept(sock, NULL, NULL);
		errif(csock==-1, "accept");
		log("Serving new connection...");

		for (;;) {
			patype *data = audio_read();
			sendall(csock, data, NFRAMES * sizeof *data);
		}

		errif(close(csock)==-1, "close csock");
		log("Closed client socket.");
	}
	errif(close(sock)==-1,  "close sock");
	log("Close server socket.");

	audio_terminate();
}
