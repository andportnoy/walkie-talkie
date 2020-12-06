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

int main(int argc, char **argv) {
	audio_initialize();

	assert(argc == 2);
	char *host = argv[1];

	int sock = client(host, PORT);
	log("Connected.");

	client_loop(sock);

	close(sock);
	log("Closed socket.");

	audio_terminate();
}
