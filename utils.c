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
