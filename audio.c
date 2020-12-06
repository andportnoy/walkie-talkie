void tableprint(char ***table, int m, int n) {
	int colwidth[n];
	memset(colwidth, 0, sizeof colwidth);
	char *(*t)[n] = (char *(*)[n]) table;

	for (int i=0; i<m; ++i)
	for (int j=0; j<n; ++j) {
		int k = strlen(t[i][j]);
		colwidth[j] = colwidth[j]<k? k: colwidth[j];
	}
	for (int i=0; i<m; ++i) {
		for (int j=0; j<n; ++j) {
			printf(j? " %*s": "%*s", colwidth[j], t[i][j]);
		}
		printf("\n");
	}

}

void devprint(void) {
	char *table[100][9] = {
		{"name", "srate,Hz", "ichans", "iminlat,ms", "imaxlat,ms",
		 "ochans", "ominlat,ms", "omaxlat,ms", "default"},
	};

	int m = 1+Pa_GetDeviceCount();
	int n = sizeof table[0]/sizeof table[0][0];
	int id = Pa_GetDefaultInputDevice();
	int od = Pa_GetDefaultOutputDevice();
	for (int i=1; i<m; ++i) {
		char buf[1024];
		int d = i-1;
		const PaDeviceInfo *dev = Pa_GetDeviceInfo(d);
		int j = 0;

		sprintf(buf, "%s", dev->name);
		table[i][j++] = strdup(buf);

		sprintf(buf, "%.0f", dev->defaultSampleRate);
		table[i][j++] = strdup(buf);

		sprintf(buf, "%d", dev->maxInputChannels);
		table[i][j++] = strdup(buf);

		sprintf(buf, "%.2f", 1000*dev->defaultLowInputLatency);
		table[i][j++] = strdup(buf);

		sprintf(buf, "%.2f", 1000*dev->defaultHighInputLatency);
		table[i][j++] = strdup(buf);

		sprintf(buf, "%d", dev->maxOutputChannels);
		table[i][j++] = strdup(buf);

		sprintf(buf, "%.2f", 1000*dev->defaultLowOutputLatency);
		table[i][j++] = strdup(buf);

		sprintf(buf, "%.2f", 1000*dev->defaultHighOutputLatency);
		table[i][j++] = strdup(buf);

		sprintf(buf, d==id? (d==od? "input,output": "input"):
				    (d==od? "output"      : ""     ));
		table[i][j++] = strdup(buf);

		assert(j == n);
	}
	tableprint((char ***)table, m, n);
}

static struct {
	PaStream *stream;
	patype *buf; /* NFRAMES */
	int initialized;
} ctx;

void audio_initialize(void) {
	ctx.buf = calloc(NFRAMES, sizeof *ctx.buf);

	pacheck(Pa_Initialize(), "initialize");
	pacheck(
	  Pa_OpenDefaultStream(&ctx.stream, 1, 1, PAFORMAT, SRATE, NFRAMES,0,0),
	  "open default stream");
	pacheck(Pa_StartStream(ctx.stream), "start stream");

	ctx.initialized = 1;
}

void audio_terminate(void) {
	pacheck(Pa_StopStream(ctx.stream), "stop stream");
	pacheck(Pa_CloseStream(ctx.stream), "close stream");
	pacheck(Pa_Terminate(), "terminate")
	free(ctx.buf);

	ctx.initialized = 0;
}

patype *audio_record(void) {
	pawarn(Pa_ReadStream(ctx.stream, ctx.buf, NFRAMES), "read stream");
	return ctx.buf;
}

void audio_play(patype *data) {
	pawarn(Pa_WriteStream(ctx.stream, data, NFRAMES), "write stream");
}
