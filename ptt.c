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
		audio_play(buf);
	}
}

void server_loop(int sock) {
	for (;;) {
		int csock = accept(sock, NULL, NULL);
		errif(csock==-1, "accept");
		log("Serving new connection...");

		for (;;) {
			patype *data = audio_record();
			sendall(csock, data, NFRAMES * sizeof *data);
		}

		errif(close(csock)==-1, "close csock");
		log("Closed client socket.");
	}
}

int recording = 0;
void *keyboard_monitor(void *arg) {
	(void) arg;
	for (;;) {
		if (getchar() == '\n') {
			recording = !recording;
			log(recording? "ON": "OFF");
		}
	}
}

void *ptt_loop(void *arg) {
	int sock = *(int *)arg;
	int flags = fcntl(sock, F_GETFL);
	errif(-1==flags, "fcntl: F_GETFL");
	errif(-1==fcntl(sock, F_SETFL, flags|O_NONBLOCK),"fcntl: F_SETFL");
	for (;;) {
		patype buf[NFRAMES] = {0};
		int x;
		while ((x = recvall(sock, buf, sizeof buf)) > 0) {
			dieif(x != sizeof buf, "received %d, expected %lu",
			  x, sizeof buf);
			audio_play(buf);
		}
		while (recording) {
			patype *chunk = audio_record();
			x = sendall(sock, chunk, NFRAMES * sizeof *chunk);
		}
		nanosleep(&(struct timespec){0, 1000000}, NULL);
	}
	return NULL;
}

int main(int argc, char **argv) {
	char *host = NULL;
	if (argc == 2)
		host = argv[1];

	pthread_t keymon;
	pthread_create(&keymon, NULL, keyboard_monitor, NULL);

	audio_initialize();

	int sock;
	if (host) {
		sock = client(host, PORT);
		log("Connected.");
	} else {
		int ssock = server(PORT);
		log("Listening for connections...");

		sock = accept(ssock, NULL, NULL);
		errif(sock == -1, "accept");
		log("Serving client...");
	}

	log("Press enter to start recording, press again to stop.");
	ptt_loop(&sock);

	errif(close(sock)==-1, "socket close");
	log("Closed socket.");

	audio_terminate();

	pthread_join(keymon, NULL);
}
