#define PAFORMAT paFloat32
#define SRATE 44100
#define NFRAMES 8192

#define pacheck(err, msg) { \
	PaError e = err; \
	if ((e) != paNoError) { \
		fprintf(stderr, "%s: %s\n", msg, Pa_GetErrorText(e)); \
		exit(EXIT_FAILURE); \
	} \
}

#define pawarn(err, msg) { \
	PaError e = err; \
	if ((e) != paNoError) { \
		fprintf(stderr, "%s: %s\n", msg, Pa_GetErrorText(e)); \
	} \
}

typedef float patype;

void audio_initialize(void);
void audio_terminate(void);
patype *audio_read(void);
void audio_write(patype *data);
