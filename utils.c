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
