#define PORT "5757"

void client_loop(int sock) {
	int x;
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

int main(int argc, char **argv) {
	char *host = NULL;
	if (argc == 2)
		host = argv[1];

	audio_initialize();

	int sock;
	if (host) {
		sock = client(host, PORT);
		log("Connected.");

		client_loop(sock);
	} else {
		int sock = server(PORT);
		log("Listening for connections...");

		server_loop(sock);
	}

	errif(close(sock)==-1, "socket close");
	log("Closed socket.");

	audio_terminate();
}
