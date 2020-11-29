char *addrstr(struct addrinfo *ai) {
	char ip[INET6_ADDRSTRLEN];
	const void *addr;
	unsigned short port;
	if (ai->ai_family == AF_INET) {
		struct sockaddr_in *sai = (void *)ai->ai_addr;
		addr = &sai->sin_addr;
		port = ntohs(sai->sin_port);
	} else if (ai->ai_family == AF_INET6) {
		struct sockaddr_in6 *sai6 = (void *)ai->ai_addr;
		addr = &sai6->sin6_addr;
		port = ntohs(sai6->sin6_port);
	}

	inet_ntop(ai->ai_family, addr, ip, sizeof ip);
	char buf[1024];
	sprintf(buf, "ip = %s, port = %d", ip, port);
	return strdup(buf);
}
