#define PORT "5757"

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

void server_loop(int sock) {
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
}

int main(void) {
	audio_initialize();

	int sock = server(PORT);
	log("Listening for connections...");

	server_loop(sock);

	errif(close(sock)==-1,  "close sock");
	log("Close server socket.");

	audio_terminate();
}
